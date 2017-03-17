/*
 * CObject.cpp
 *
 *  Created on: 2014年12月2日
 *      Author: jugo
 */

#pragma once

#include <stdio.h>
#include <sys/types.h>

class CMessageHandler;

struct EVENT_EXTERNAL
{
	int m_nMsgId;
	int m_nEventFilter;
	int m_nEventRecvCommand;
	int m_nEventDisconnect;
	int m_nEventConnect;
	void init()
	{
		m_nMsgId = -1;
		m_nEventFilter = -1;
		m_nEventRecvCommand = -1;
		m_nEventDisconnect = -1;
		m_nEventConnect = -1;
	}
	bool isValid()
	{
		if(-1 != m_nMsgId && -1 != m_nEventFilter && -1 != m_nEventRecvCommand)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class CObject
{
public:
	explicit CObject();
	virtual ~CObject();
	void clearMessage();
	int sendMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);
	void _OnTimer(int nId);
	int initMessage(int nKey);
	int run(int nRecvEvent, const char * szDescript = 0);
	timer_t setTimer(int nId, int nSecStart, int nInterSec, int nEvent = -1);
	void killTimer(int nId);
	unsigned long int createThread(void* (*entry)(void*), void* arg);
	void threadJoin(unsigned long int thdid);
	void threadExit();
	int threadCancel(unsigned long int thread);
	unsigned long int getThreadID();
	void closeMsq();

protected:
	// virtual function
	virtual void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
	{
		printf("[CObject] onReceiveMessage: event=%d command=%d id=%lu data_length=%d, data=%x\n", nEvent, nCommand,
				nId, nDataLen, *(unsigned int *) pData);
	}
	;

	virtual void onTimer(int nId)
	{
		printf("[CObject] onTimer Id:%d\n", nId);
	}
	;

private:
	CMessageHandler *messageHandler;
	int mnTimerEventId;

};
