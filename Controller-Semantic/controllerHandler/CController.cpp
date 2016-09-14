/*
 * CController.cpp
 *
 *  Created on: 2016年06月27日
 *      Author: Jugo
 */

#include <list>
#include <ctime>

#include "common.h"
#include "CSocketServer.h"
#include "CSocketClient.h"
#include "event.h"
#include "packet.h"
#include "utility.h"
#include "CCmpHandler.h"
#include "CController.h"
#include "CDataHandler.cpp"
#include "CSqliteHandler.h"
#include "CThreadHandler.h"

using namespace std;

static CController * controller = 0;

/**
 * Define Socket Client ReceiveFunction
 */
int ClientReceive(int nSocketFD, int nDataLen, const void *pData)
{
	//controlcenter->receiveCMP(nSocketFD, nDataLen, pData);
	return 0;
}

/**
 *  Define Socket Server Receive Function
 */
int ServerReceive(int nSocketFD, int nDataLen, const void *pData)
{
	//controlcenter->receiveClientCMP(nSocketFD, nDataLen, pData);
	return 0;
}

CController::CController() :
		CObject(), cmpServer(new CSocketServer), cmpParser(CCmpHandler::getInstance()), sqlite(
				CSqliteHandler::getInstance()), cmpClient(new CSocketClient)
{
	for (int i = 0; i < MAX_FUNC_POINT; ++i)
	{
		cmpRequest[i] = &CController::cmpUnknow;
	}

}

CController::~CController()
{
	delete cmpParser;
}

CController* CController::getInstance()
{
	if (0 == controller)
	{
		controller = new CController();
	}
	return controller;
}

int CController::startSqlite(const int nDBId, const std::string strDB)
{
	int nResult = FALSE;
	switch (nDBId)
	{
	case DB_CONTROLLER:
		nResult = sqlite->openControllerDB(strDB.c_str());
		break;
	case DB_USER:
		nResult = sqlite->openUserDB(strDB.c_str());
		break;
	case DB_IDEAS:
		nResult = sqlite->openIdeasDB(strDB.c_str());
		break;
	case DB_MDM:
		nResult = sqlite->openMdmDB(strDB.c_str());
		break;
	}

	return nResult;
}

void CController::onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
{
	switch (nCommand)
	{
	case EVENT_COMMAND_SOCKET_TCP_RECEIVE:
		onCMP(nId, nDataLen, pData);
		break;
	case EVENT_COMMAND_SOCKET_CLIENT_CONNECT:
		_log("[Controller] Socket Client FD:%d Connected", (int) nId);
		break;
	case EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT:
		_log("[Controller] Socket Client FD:%d Close", (int) nId);
		break;
	default:
		_log("[Controller] Unknow message command: %d", nCommand);
		break;
	}
}

int CController::startServer(const int nPort, const int nMsqId)
{
	if (0 >= nPort || 0 >= nMsqId)
		return FALSE;

	/** Run socket server for CMP **/
	if (0 < nMsqId)
	{
		cmpServer->setPackageReceiver(nMsqId, EVENT_FILTER_CONTROLLER, EVENT_COMMAND_SOCKET_TCP_RECEIVE);
		cmpServer->setClientConnectCommand(EVENT_COMMAND_SOCKET_CLIENT_CONNECT);
		cmpServer->setClientDisconnectCommand( EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT);
	}

	if ( FAIL == cmpServer->start( AF_INET, NULL, nPort))
	{
		_log("CMP Server Socket Create Fail");
		return FALSE;
	}

	return TRUE;
}

void CController::stopServer()
{
	if (cmpServer)
	{
		cmpServer->stop();
		delete cmpServer;
		cmpServer = 0;
	}
}

int CController::sendCommand(int nSocket, int nCommand, int nStatus, int nSequence, bool isResp)
{
	int nRet = -1;
	int nCommandSend;
	CMP_HEADER cmpHeader;
	void *pHeader = &cmpHeader;

	memset(&cmpHeader, 0, sizeof(CMP_HEADER));
	nCommandSend = nCommand;

	if (isResp)
	{
		nCommandSend = generic_nack | nCommand;
	}

	cmpParser->formatHeader(nCommandSend, nStatus, nSequence, &pHeader);
	nRet = cmpServer->socketSend(nSocket, &cmpHeader, sizeof(CMP_HEADER));
	printPacket(nCommandSend, nStatus, nSequence, nRet, "[Center Send]", nSocket);
	return nRet;
}

void CController::ackPacket(int nClientSocketFD, int nCommand, const void * pData)
{
	string strLog;
	switch (nCommand)
	{
	case generic_nack:
		break;
	case bind_response:
		break;
	case authentication_response:
		break;
	case access_log_response:
		break;
	case enquire_link_response:
		for (vector<int>::iterator it = vEnquireLink.begin(); it != vEnquireLink.end(); ++it)
		{
			if (nClientSocketFD == *it)
			{
				vEnquireLink.erase(it);
				break;
			}
		}
		break;
	case unbind_response:
		break;
	case update_response:
		break;
	case reboot_response:
		break;
	case config_response:
		break;
	case power_port_set_response:
		break;
	}
}

int CController::cmpUnknow(int nSocket, int nCommand, int nSequence, const void * pData)
{
	_DBG("[Controller] Unknow command:%d", nCommand);
	sendCommand(nSocket, nCommand, STATUS_RINVCMDID, nSequence, true);
	return 0;
}

int CController::cmpResponse(const int nSocket, const int nCommandId, const int nSequence, const char * szData)
{
	int nRet = -1;
	int nBody_len = 0;
	int nTotal_len = 0;

	CMP_PACKET packet;
	void *pHeader = &packet.cmpHeader;
	char *pIndex = packet.cmpBody.cmpdata;

	memset(&packet, 0, sizeof(CMP_PACKET));

	cmpParser->formatHeader(nCommandId, STATUS_ROK, nSequence, &pHeader);
	memcpy(pIndex, szData, strlen(szData));
	pIndex += strlen(szData);
	nBody_len += strlen(szData);
	memcpy(pIndex, "\0", 1);
	pIndex += 1;
	nBody_len += 1;

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl(nTotal_len);

	nRet = cmpServer->socketSend(nSocket, &packet, nTotal_len);
	printPacket(nCommandId, STATUS_ROK, nSequence, nRet, "[Controller] cmpResponse", nSocket);

	string strLog;
	if (0 >= nRet)
	{
		_log("[Controller] cmpResponse Fail socket: %d", nSocket);
	}

	return nRet;
}

void CController::onCMP(int nClientFD, int nDataLen, const void *pData)
{
	_DBG("[Controller] Receive CMP From Client:%d Length:%d", nClientFD, nDataLen);

	int nRet = -1;
	int nPacketLen = 0;
	CMP_HEADER cmpHeader;
	char *pPacket;

	pPacket = (char*) const_cast<void*>(pData);
	memset(&cmpHeader, 0, sizeof(CMP_HEADER));

	cmpHeader.command_id = cmpParser->getCommand(pPacket);
	cmpHeader.command_length = cmpParser->getLength(pPacket);
	cmpHeader.command_status = cmpParser->getStatus(pPacket);
	cmpHeader.sequence_number = cmpParser->getSequence(pPacket);

	printPacket(cmpHeader.command_id, cmpHeader.command_status, cmpHeader.sequence_number, cmpHeader.command_length,
			"[Center Recv]", nClientFD);

	if (cmpParser->isAckPacket(cmpHeader.command_id))
	{
		ackPacket(nClientFD, cmpHeader.command_id, pPacket);
		return;
	}

	if (0x000000FF < cmpHeader.command_id)
	{
		sendCommand(nClientFD, cmpHeader.command_id, STATUS_RINVCMDID, cmpHeader.sequence_number, true);
		return;
	}

	(this->*this->cmpRequest[cmpHeader.command_id])(nClientFD, cmpHeader.command_id, cmpHeader.sequence_number,
			pPacket);

}
