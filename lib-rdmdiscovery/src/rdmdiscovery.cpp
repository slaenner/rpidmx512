/**
 * @file rdmddiscovery.cpp
 *
 */
/* Copyright (C) 2017-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#ifndef NDEBUG
#include <stdio.h>
#endif

#include "rdm.h"
#include "rdm_e120.h"
#include "rdmdiscovery.h"

#include "hardware.h"

extern "C" {
 void udelay(uint32_t us);
}

static uint8_t pdl[2][RDM_UID_SIZE];

typedef union cast {
	uint64_t uint;
	uint8_t uid[RDM_UID_SIZE];
} _cast;

static _cast uuid_cast;

#define RECEIVE_TIME_OUT (2800*100) //TODO #define

RDMDiscovery::RDMDiscovery(uint8_t nPort) : m_nPort(nPort) {
	m_UnMute.SetDstUid(UID_ALL);
	m_UnMute.SetCc(E120_DISCOVERY_COMMAND);
	m_UnMute.SetPid(E120_DISC_UN_MUTE);

	m_Mute.SetCc(E120_DISCOVERY_COMMAND);
	m_Mute.SetPid(E120_DISC_MUTE);

	m_DiscUniqueBranch.SetDstUid(UID_ALL);
	m_DiscUniqueBranch.SetCc(E120_DISCOVERY_COMMAND);
	m_DiscUniqueBranch.SetPid(E120_DISC_UNIQUE_BRANCH);
}

void RDMDiscovery::SetUid(const uint8_t *uid) {
	memcpy(m_Uid, uid, RDM_UID_SIZE);

	m_UnMute.SetSrcUid(uid);
	m_Mute.SetSrcUid(uid);
	m_DiscUniqueBranch.SetSrcUid(uid);
}

const uint8_t *RDMDiscovery::GetUid(void) {
	return m_Uid;
}

void RDMDiscovery::Full(void) {
	Reset();

	m_UnMute.Send(m_nPort);
	m_UnMute.ReceiveTimeOut(m_nPort, 2800);

	Hardware::Get()->WatchdogFeed();
	udelay(100000);

	m_UnMute.Send(m_nPort);
	m_UnMute.ReceiveTimeOut(m_nPort, 2800);

	Hardware::Get()->WatchdogFeed();
	udelay(100000);

	m_UnMute.Send(m_nPort);

	const struct TRdmMessage *pResponse = reinterpret_cast<const struct TRdmMessage*>(m_Mute.ReceiveTimeOut(m_nPort, RECEIVE_TIME_OUT));

	if (pResponse != 0) {
		FindDevices(ConvertUid(pResponse->source_uid), ConvertUid(pResponse->source_uid));
	}

	FindDevices(0x000000000000, 0xfffffffffffe);

	Dump();
}

bool RDMDiscovery::FindDevices(uint64_t LowerBound, uint64_t UpperBound) {
	struct TRdmMessage *pRdmMessage;
	uint8_t uid[RDM_UID_SIZE];
	bool bDeviceFound = false;
	uint64_t MidPosition;

	Hardware::Get()->WatchdogFeed();

#ifndef NDEBUG
	printf("FindDevices : ");
	PrintUid(LowerBound);
	printf(" - ");
	PrintUid(UpperBound);
	printf("\n");
#endif

	if (LowerBound == UpperBound) {
		memcpy(uid, ConvertUid(LowerBound), RDM_UID_SIZE);

		m_Mute.SetDstUid(uid);
		m_Mute.Send(m_nPort);

		pRdmMessage = reinterpret_cast<struct TRdmMessage*>(const_cast<uint8_t*>(m_Mute.ReceiveTimeOut(m_nPort, RECEIVE_TIME_OUT)));

		if (pRdmMessage != 0) {
			if ((pRdmMessage->command_class == E120_DISCOVERY_COMMAND_RESPONSE) && (memcmp(uid, pRdmMessage->source_uid, RDM_UID_SIZE) == 0)) {
				AddUid(uid);
			}
		} else {
			return true;
		}
	} else {
		memcpy(pdl[0], ConvertUid(LowerBound), RDM_UID_SIZE);
		memcpy(pdl[1], ConvertUid(UpperBound), RDM_UID_SIZE);

		m_DiscUniqueBranch.SetPd(reinterpret_cast<const uint8_t*>(pdl), 2 * RDM_UID_SIZE);
		m_DiscUniqueBranch.Send(m_nPort);

		pRdmMessage = reinterpret_cast<struct TRdmMessage*>(const_cast<uint8_t*>(m_DiscUniqueBranch.ReceiveTimeOut(m_nPort, RECEIVE_TIME_OUT)));

		if (pRdmMessage != 0) {
			bDeviceFound = true;

			if (IsValidDiscoveryResponse(reinterpret_cast<const uint8_t*>(pRdmMessage), uid)) {
				bDeviceFound = QuickFind(uid);
			}

			if (bDeviceFound) {
				MidPosition = ((LowerBound & (0x0000800000000000 - 1)) + (UpperBound & (0x0000800000000000 - 1))) / 2
						+ (UpperBound & (0x0000800000000000) ? 0x0000400000000000 : 0 )
						+ (LowerBound & (0x0000800000000000) ? 0x0000400000000000 : 0 );

				bDeviceFound = FindDevices(LowerBound, MidPosition);
				bDeviceFound |= FindDevices(MidPosition + 1, UpperBound);

				if (bDeviceFound) {
					return true;
				}
			}
		}
	}

	return false;
}

const uint8_t *RDMDiscovery::ConvertUid(uint64_t uid) {
	uuid_cast.uint = __builtin_bswap64(uid << 16);
	return uuid_cast.uid;
}

uint64_t RDMDiscovery::ConvertUid(const uint8_t *uid) {
	memcpy(uuid_cast.uid, uid, RDM_UID_SIZE);
	return __builtin_bswap64(uuid_cast.uint << 16);
}

void RDMDiscovery::PrintUid(__attribute__((unused)) uint64_t uid) {
#ifndef NDEBUG
	PrintUid(ConvertUid(uid));
#endif
}

void RDMDiscovery::PrintUid(__attribute__((unused)) const uint8_t *uid) {
#ifndef NDEBUG
	printf("%.2x%.2x:%.2x%.2x%.2x%.2x", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5]);
#endif
}

bool RDMDiscovery::IsValidDiscoveryResponse(const uint8_t *response, uint8_t *uid) {
	uint8_t checksum[2];
	uint16_t rdm_checksum = 6 * 0xFF;
	bool bIsValid = false;

	if (response[0] == 0xFE) {
		uid[0] = response[8] & response[9];
		uid[1] = response[10] & response[11];

		uid[2] = response[12] & response[13];
		uid[3] = response[14] & response[15];
		uid[4] = response[16] & response[17];
		uid[5] = response[18] & response[19];

		checksum[0] = response[22] & response[23];
		checksum[1] = response[20] & response[21];

		for (unsigned i = 0; i < 6; i++) {
			rdm_checksum += uid[i];
		}

		if (((rdm_checksum >> 8) == checksum[1]) && ((rdm_checksum & 0xFF) == checksum[0])) {
			bIsValid = true;
		}

#ifndef NDEBUG
		PrintUid(uid);
		printf(", checksum %.2x%.2x -> %.4x {%c}\n", checksum[1], checksum[0], rdm_checksum, bIsValid ? 'Y' : 'N');
#endif

	} else {
#ifndef NDEBUG
		printf("Not a valid response [%.2x]\n", response[0]);
#endif
	}

	return bIsValid;
}

bool RDMDiscovery::QuickFind(const uint8_t *uid) {
	uint8_t r_uid[RDM_UID_SIZE];

	Hardware::Get()->WatchdogFeed();

#ifndef NDEBUG
	printf("QuickFind : ");
	PrintUid(uid);
	printf("\n");
#endif

	m_Mute.SetDstUid(uid);
	m_Mute.Send(m_nPort);

	uint8_t *pResponse = const_cast<uint8_t*>(m_Mute.ReceiveTimeOut(m_nPort, RECEIVE_TIME_OUT));

	if (pResponse != 0) {
		struct TRdmMessage *pRdmMessage = reinterpret_cast<struct TRdmMessage*>(pResponse);

		if ((pRdmMessage->command_class == E120_DISCOVERY_COMMAND_RESPONSE) && (memcmp(uid, pRdmMessage->source_uid, RDM_UID_SIZE) == 0)) {
			AddUid(uid);
		}
	}

	Hardware::Get()->WatchdogFeed();

	m_DiscUniqueBranch.Send(m_nPort);

	pResponse = const_cast<uint8_t *>(m_DiscUniqueBranch.ReceiveTimeOut(m_nPort, RECEIVE_TIME_OUT));

	if ((pResponse != 0) && (IsValidDiscoveryResponse(pResponse, r_uid))) {
		QuickFind(r_uid);
	} else {
		return true;
	}

	return false;
}
