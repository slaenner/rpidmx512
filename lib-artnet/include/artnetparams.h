/**
 * @file artnetparams.h
 *
 */
/**
 * Art-Net Designed by and Copyright Artistic Licence Holdings Ltd.
 */
/* Copyright (C) 2016-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"); to deal
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

#ifndef ARTNETPARAMS_H_
#define ARTNETPARAMS_H_

#include <stdint.h>
#include <time.h>

#include "artnetnode.h"

#include "lightset.h"

struct TArtNetParams {
	uint32_t nSetList;										///< 4	  4
	uint8_t nNet;											///< 1	  5
	uint8_t nSubnet;										///< 1	  6
	uint8_t nUniverse;										///< 1	  7
	TLightSetOutputType tOutputType;						///< 1	  8
	bool bUseTimeCode;										///< 1	  9
	bool bUseTimeSync;										///< 1	 10
	bool bEnableRdm;										///< 1	 11
	bool bRdmDiscovery;										///< 1	 12
	uint8_t aShortName[artnet::SHORT_NAME_LENGTH];			///< 18	 30
	uint8_t aLongName[artnet::LONG_NAME_LENGTH];			///< 64	 94
	uint16_t nMultiPortOptions;								///< 2	 96
	uint8_t aOemValue[2];									///< 2	 98
	time_t nNetworkTimeout;									///< 4	102
	bool bDisableMergeTimeout;								///< 1	103
	uint8_t nUniversePort[artnet::MAX_PORTS];				///< 4	107
	uint8_t nMergeMode;										///< 1	108
	uint8_t nMergeModePort[artnet::MAX_PORTS];				///< 4	112
	uint8_t nProtocol;										///< 1	113
	uint8_t nProtocolPort[artnet::MAX_PORTS];				///< 4	117
	bool bEnableNoChangeUpdate;								///< 1	118
	uint8_t nDirection;										///< 1	119
	uint32_t nDestinationIpPort[artnet::MAX_PORTS];			///< 16	135
#if defined (__linux__)
}__attribute__((packed));
#else
};													///< Not packed!
#endif

struct ArtnetParamsMaskMultiPortOptions {
	static constexpr auto DESTINATION_IP_A = (1U << 0);
	static constexpr auto DESTINATION_IP_B = (1U << 1);
	static constexpr auto DESTINATION_IP_C = (1U << 2);
	static constexpr auto DESTINATION_IP_D = (1U << 3);
};

struct ArtnetParamsMask {
	static constexpr auto LONG_NAME = (1U << 0);
	static constexpr auto SHORT_NAME = (1U << 1);
	static constexpr auto NET = (1U << 2);
	static constexpr auto SUBNET = (1U << 3);
	static constexpr auto UNIVERSE = (1U << 4);
	static constexpr auto RDM = (1U << 5);
	static constexpr auto TIMECODE = (1U << 6);
	static constexpr auto TIMESYNC = (1U << 7);
	static constexpr auto OUTPUT = (1U << 8);
	//static constexpr auto NOT_USED = (1U << 9); // aManufacturerId
	static constexpr auto OEM_VALUE = (1U << 10);
	static constexpr auto NETWORK_TIMEOUT = (1U << 11);
	static constexpr auto MERGE_TIMEOUT = (1U << 12);
	static constexpr auto UNIVERSE_A = (1U << 13);
	static constexpr auto UNIVERSE_B = (1U << 14);
	static constexpr auto UNIVERSE_C = (1U << 15);
	static constexpr auto UNIVERSE_D = (1U << 16);
	static constexpr auto MERGE_MODE = (1U << 17);
	static constexpr auto MERGE_MODE_A = (1U << 18);
	static constexpr auto MERGE_MODE_B = (1U << 19);
	static constexpr auto MERGE_MODE_C = (1U << 20);
	static constexpr auto MERGE_MODE_D = (1U << 21);
	static constexpr auto PROTOCOL = (1U << 22);
	static constexpr auto PROTOCOL_A = (1U << 23);
	static constexpr auto PROTOCOL_B = (1U << 24);
	static constexpr auto PROTOCOL_C = (1U << 25);
	static constexpr auto PROTOCOL_D = (1U << 26);
	static constexpr auto ENABLE_NO_CHANGE_OUTPUT = (1U << 27);
	static constexpr auto DIRECTION = (1U << 28);
};

class ArtNetParamsStore {
public:
	virtual ~ArtNetParamsStore() {
	}

	virtual void Update(const struct TArtNetParams *pArtNetParams)=0;
	virtual void Copy(struct TArtNetParams *pArtNetParams)=0;
};

class ArtNetParams {
public:
	ArtNetParams(ArtNetParamsStore *pArtNetParamsStore = nullptr);

	bool Load();
	void Load(const char *pBuffer, uint32_t nLength);

	void Builder(const struct TArtNetParams *pArtNetParams, char *pBuffer, uint32_t nLength, uint32_t &nSize);
	void Save(char *pBuffer, uint32_t nLength, uint32_t &nSize);

	void Set(ArtNetNode *);

	void Dump();

	uint8_t GetNet() {
		return m_tArtNetParams.nNet;
	}

	uint8_t GetSubnet() {
		return m_tArtNetParams.nSubnet;
	}

	uint8_t GetUniverse() {
		return m_tArtNetParams.nUniverse;
	}

	const uint8_t *GetShortName() {
		return m_tArtNetParams.aShortName;
	}

	const uint8_t *GetLongName() {
		return m_tArtNetParams.aLongName;
	}

	TLightSetOutputType GetOutputType() {
		return m_tArtNetParams.tOutputType;
	}

	time_t GetNetworkTimeout() {
		return m_tArtNetParams.nNetworkTimeout;
	}

	bool IsUseTimeCode() {
		return m_tArtNetParams.bUseTimeCode;
	}

	bool IsUseTimeSync() {
		return m_tArtNetParams.bUseTimeSync;
	}

	bool IsRdm() {
		return m_tArtNetParams.bEnableRdm;
	}

	bool IsRdmDiscovery() {
		return m_tArtNetParams.bRdmDiscovery;
	}

	uint8_t GetUniverse(uint8_t nPort, bool &IsSet);

	bool IsEnableNoChangeUpdate() {
		return m_tArtNetParams.bEnableNoChangeUpdate;
	}

	TArtNetPortDir GetDirection() {
		return static_cast<TArtNetPortDir>(m_tArtNetParams.nDirection);
	}

public:
	static void staticCallbackFunction(void *p, const char *s);

private:
	void callbackFunction(const char *pLine);
	bool isMaskSet(uint32_t nMask) {
		return (m_tArtNetParams.nSetList & nMask) == nMask;
	}
	bool isMaskMultiPortOptionsSet(uint16_t nMask) {
		return (m_tArtNetParams.nMultiPortOptions & nMask) == nMask;
	}

private:
	ArtNetParamsStore *m_pArtNetParamsStore;
	struct TArtNetParams m_tArtNetParams;
};

#endif /* ARTNETPARAMS_H_ */
