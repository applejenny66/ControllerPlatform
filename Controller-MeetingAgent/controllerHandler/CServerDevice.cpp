

#include "CServerDevice.h"
#include "IReceiver.h"
#include "event.h"
#include "common.h"
#include "CCmpHandler.h"
#include "CDataHandler.cpp"
#include "JSONObject.h"
#include "ICallback.h"
#include "CTimer.h"
#include "packet.h"

static CServerDevice * serverDevice = 0;

CServerDevice::CServerDevice() :
		CSocketServer(), cmpParser(CCmpHandler::getInstance())
{
	mapFunc[fcm_id_register_request] = &CServerDevice::cmpFCMIdRegister;
	mapFunc[facebook_token_client_request] = &CServerDevice::cmpFBToken;
	mapFunc[smart_building_qrcode_tokn_request] = &CServerDevice::cmpQRCodeToken;
	mapFunc[smart_building_appversion_request] = &CServerDevice::cmpAPPVersion;
	mapFunc[smart_building_getmeetingdata_request] = &CServerDevice::cmpGetMeetingData;
	mapFunc[smart_building_amx_control_access_request] = &CServerDevice::cmpAMXControlAccess;
	mapFunc[smart_building_wireless_power_charge_request] = &CServerDevice::cmpWirelessPowerCharge;

}

CServerDevice::~CServerDevice()
{
	stop();
	mapClient.clear();
}

CServerDevice * CServerDevice::getInstance()
{
	if (0 == serverDevice)
	{
		serverDevice = new CServerDevice();
	}
	return serverDevice;
}

int CServerDevice::startServer(string strIP, const int nPort, const int nMsqId)
{
	if (0 >= nPort || 0 >= nMsqId)
		return FALSE;

	/** Run socket server for CMP **/
	if (0 < nMsqId)
	{
		setPackageReceiver(nMsqId, EVENT_FILTER_CONTROLLER, EVENT_COMMAND_SOCKET_TCP_DEVICE_RECEIVE);
		setClientConnectCommand(EVENT_COMMAND_SOCKET_CLIENT_CONNECT_DEVICE);
		setClientDisconnectCommand(EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT_DEVICE);
	}

	/** Set Receive , Packet is CMP , Message Queue Handle **/
	setPacketConf(PK_CMP, PK_MSQ);

	const char* cszAddr = NULL;
	if (!strIP.empty())
		cszAddr = strIP.c_str();

	if (FAIL == start(AF_INET, cszAddr, nPort))
	{
		_log("[CServerDevice] Socket Create Fail");
		return FALSE;
	}

	return TRUE;
}

void CServerDevice::stopServer()
{
	stop();
}

void CServerDevice::onReceive(const int nSocketFD, const void *pData)
{
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
			"[CServerDevice] Recv ", nSocketFD);

	if (cmpParser->isAckPacket(cmpHeader.command_id))
	{
		return;
	}

	map<int, MemFn>::iterator iter;
	iter = mapFunc.find(cmpHeader.command_id);

	if (0x000000FF < cmpHeader.command_id || 0x00000000 >= cmpHeader.command_id || mapFunc.end() == iter)
	{
		sendPacket(dynamic_cast<CSocket*>(serverDevice), nSocketFD, generic_nack | cmpHeader.command_id,
		STATUS_RINVCMDID, cmpHeader.sequence_number, 0);

		return;
	}

	(this->*this->mapFunc[cmpHeader.command_id])(nSocketFD, cmpHeader.command_id, cmpHeader.sequence_number, pPacket);

}

int CServerDevice::cmpFCMIdRegister(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpFCMIdRegister");

	return TRUE;
}

int CServerDevice::cmpFBToken(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpFBToken");
	return TRUE;
}

int CServerDevice::cmpQRCodeToken(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpQRCodeToken");

	return TRUE;
}

int CServerDevice::cmpAPPVersion(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpAPPVersion");

	return TRUE;
}

int CServerDevice::cmpGetMeetingData(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpGetMeetingData");

	return TRUE;
}

int CServerDevice::cmpAMXControlAccess(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpAMXControlAcess");
;
	return TRUE;
}

int CServerDevice::cmpWirelessPowerCharge(int nSocket, int nCommand, int nSequence, const void *pData)
{
	_DBG("[CServerDevice]cmpWirelessPowerCharge");

	return TRUE;
}
