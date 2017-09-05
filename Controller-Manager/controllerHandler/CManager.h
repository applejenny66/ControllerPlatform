/*
 * CManager.h
 *
 *  Created on: 2017年8月18日
 *      Author: root
 */

#pragma once

#include <string>
#include <set>
#include "CCmpServer.h"

class CProcessManager;

typedef struct _MONITOR
{
	int nThread_Max;
	_MONITOR()
	{
		nThread_Max = 30;
	}
} MONITOR;

enum
{
	PROC_NOT_RUN = 0, PROC_THREAD_OVER
};

class CManager: public CCmpServer
{
public:
	explicit CManager(CObject *object);
	virtual ~CManager();
	int checkProcess();
	void psInstanceDump(int pid);
	void setThreadMax(int nMax);
	void addProcess(const char *szProcessName);
	int getProcessCount();

private:
	CObject *mpController;
	CProcessManager *processmanager;

private:
	MONITOR monitor;
	std::set<std::string> setProcessName;
};
