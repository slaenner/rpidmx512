/**
 * @file tftpdaemon.h
 *
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef TFTPDAEMON_H_
#define TFTPDAEMON_H_

#include <stdint.h>

enum class TFTPMode {
	BINARY,
	ASCII
};

class TFTPDaemon {
public:
	TFTPDaemon(void);
	virtual ~TFTPDaemon(void);

	bool Run(void);

	virtual bool FileOpen(const char *pFileName, TFTPMode tMode)=0;
	virtual bool FileCreate(const char *pFileName, TFTPMode tMode)=0;
	virtual bool FileClose(void)=0;
	virtual size_t FileRead(void *pBuffer, size_t nCount, unsigned nBlockNumber)=0;
	virtual size_t FileWrite(const void *pBuffer, size_t nCount, unsigned nBlockNumber)=0;

	virtual void Exit(void)=0;

private:
	void HandleRequest(void);
	void HandleRecvAck(void);
	void HandleRecvData(void);
	void SendError (uint16_t usErrorCode, const char *pErrorMessage);
	void DoRead(void);
	void DoWriteAck(void);

private:
	enum class TFTPState {
		INIT,
		WAITING_RQ,
		RRQ_SEND_PACKET,
		RRQ_RECV_ACK,
		WRQ_SEND_ACK,
		WRQ_RECV_PACKET
	};
	TFTPState m_nState;
	int m_nIdx;
	uint8_t m_Buffer[528];
	uint32_t m_nFromIp;
	uint16_t m_nFromPort;
	size_t m_nLength;
	uint16_t m_nBlockNumber;
	size_t m_nDataLength;
	uint16_t m_nPacketLength;
	bool m_bIsLastBlock;

	static TFTPDaemon* Get(void) {
		return s_pThis;
	}

private:
	static TFTPDaemon *s_pThis;
};

#endif /* TFTPDAEMON_H_ */
