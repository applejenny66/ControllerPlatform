/*
 *  CServerCenter.h
 *
 *  Created on: 2016-12-05
 *  Author: Jugo
 */

#pragma once

#include <string>
#include <map>

#include "CSocketServer.h"
#include "ICallback.h"

using namespace std;

class CCmpHandler;
class CSqliteHandler;

class CServerCenter: public CSocketServer
{
public:
	void onReceive(const int nSocketFD, const void *pData);
public:
	static CServerCenter * getInstance();
	virtual ~CServerCenter();
	int startServer(string strIP, const int nPort, const int nMsqId);
	void stopServer();
	void setCallback(const int nId, CBFun cbfun);
	void broadcastAMXStatus(string strStatus);
	void addClient(const int nSocketFD);
	void deleteClient(const int nSocketFD);

private:
	explicit CServerCenter();
	CCmpHandler *cmpParser;
	typedef int (CServerCenter::*MemFn)(int, int, int, const void *);
	map<int, MemFn> mapFunc;
	int cmpBind(int nSocket, int nCommand, int nSequence, const void *pData);
	int cmpUnbind(int nSocket, int nCommand, int nSequence, const void *pData);
	int cmpInitial(int nSocket, int nCommand, int nSequence, const void *pData);
	int cmpSignup(int nSocket, int nCommand, int nSequence, const void *pData);
	map<int, CBFun> mapCallback;
	map<int, int> mapClient;
	CSqliteHandler *sqlite;

};