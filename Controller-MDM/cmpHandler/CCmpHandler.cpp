/*
 * CWmpHandler.cpp
 *
 *  Created on: 2015年10月21日
 *      Author: Louis Ju
 */

#include "CCmpHandler.h"

#include <string.h>

#include "utility.h"
#include "common.h"
#include "packet.h"
#include "CDataHandler.cpp"
#include "LogHandler.h"

static CCmpHandler* cmpHandler = 0;

CCmpHandler::CCmpHandler()
{

}

CCmpHandler::~CCmpHandler()
{

}

CCmpHandler* CCmpHandler::getInstance()
{
	if (0 == cmpHandler)
	{
		cmpHandler = new CCmpHandler();
	}
	return cmpHandler;
}

int CCmpHandler::getCommand(const void *pData)
{
	int nCommand;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nCommand = ntohl(pHeader->command_id);
	return nCommand;
}

int CCmpHandler::getLength(const void *pData)
{
	int nLength;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nLength = ntohl(pHeader->command_length);
	return nLength;
}

int CCmpHandler::getStatus(const void *pData)
{
	int nStatus;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nStatus = ntohl(pHeader->command_status);
	return nStatus;
}

int CCmpHandler::getSequence(const void *pData)
{
	int nSequence;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nSequence = ntohl(pHeader->sequence_number);
	return nSequence;
}

void CCmpHandler::formatRespPacket(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	int nCommand_resp;
	nCommand_resp = generic_nack | nCommand;
	((CMP_HEADER*) *pHeader)->command_id = htonl(nCommand_resp);
	((CMP_HEADER*) *pHeader)->command_status = htonl(nStatus);
	((CMP_HEADER*) *pHeader)->sequence_number = htonl(nSequence);
	((CMP_HEADER*) *pHeader)->command_length = htonl(sizeof(CMP_HEADER));
}

void CCmpHandler::formatReqPacket(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	((CMP_HEADER*) *pHeader)->command_id = htonl(nCommand);
	((CMP_HEADER*) *pHeader)->command_status = htonl(nStatus);
	((CMP_HEADER*) *pHeader)->sequence_number = htonl(nSequence);
	((CMP_HEADER*) *pHeader)->command_length = htonl(sizeof(CMP_HEADER));
}

void CCmpHandler::formatHeader(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	((CMP_HEADER*) *pHeader)->command_id = htonl(nCommand);
	((CMP_HEADER*) *pHeader)->command_status = htonl(nStatus);
	((CMP_HEADER*) *pHeader)->sequence_number = htonl(nSequence);
	((CMP_HEADER*) *pHeader)->command_length = htonl(sizeof(CMP_HEADER));
}

int CCmpHandler::formatPacket(int nCommand, void ** pPacket, int nBodyLen)
{
	int nTotal_len;
	static int seqnum = 0;
	CMP_HEADER wmpHeader;

	wmpHeader.command_id = htonl(nCommand);
	wmpHeader.command_status = htonl(0);
	wmpHeader.sequence_number = htonl(++seqnum);
	nTotal_len = sizeof(CMP_HEADER) + nBodyLen;
	wmpHeader.command_length = htonl(nTotal_len);
	memcpy(*pPacket, &wmpHeader, sizeof(CMP_HEADER));

	return nTotal_len;
}

void CCmpHandler::getSourcePath(const void *pData, char **pPath)
{
	char* pBody;

	pBody = (char*) ((char *) pData + sizeof(CMP_HEADER));
	if (0 != pBody)
	{
		strcpy(*pPath, pBody);
	}
}

int CCmpHandler::parseBody(int nCommand, const void *pData, CDataHandler<std::string> &rData)
{
	int nRet = 0;
	int nStrLen = 0;
	int nTotalLen = 0;
	int nBodyLen = 0;
	int nIndex = 0;
	int nPort = 0;
	int *pType;
	int nType = 0;
	char * pBody;
	char temp[MAX_SIZE];
	int nMacCount = 0;
	char macName[13];

	nTotalLen = getLength(pData);
	nBodyLen = nTotalLen - sizeof(CMP_HEADER);

	if (0 < nBodyLen)
	{
		pBody = (char*) ((char *) const_cast<void*>(pData) + sizeof(CMP_HEADER));

		switch (nCommand)
		{
		case bind_request:
			if (isValidStr((const char*) pBody, MAX_SIZE))
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, pBody);
				rData.setData("id", temp);
				nStrLen = strlen(temp);
				++nStrLen;
				pBody += nStrLen;
			}
			break;
		case power_port_set_request:
			memset(temp, 0, sizeof(temp));
			memcpy(temp, pBody, 1);
			++pBody;
			rData.setData("wire", temp);

			memset(temp, 0, sizeof(temp));
			memcpy(temp, pBody, 1);
			++pBody;
			rData.setData("port", temp);

			memset(temp, 0, sizeof(temp));
			memcpy(temp, pBody, 1);
			++pBody;
			rData.setData("state", temp);

			if (isValidStr((const char*) pBody, MAX_SIZE))
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, pBody);
				rData.setData("controller", temp);
				nStrLen = strlen(temp);
				++nStrLen;
				pBody += nStrLen;
			}
			break;
		case power_port_state_request:
			memset(temp, 0, sizeof(temp));
			memcpy(temp, pBody, 1);
			++pBody;
			rData.setData("wire", temp);

			if (isValidStr((const char*) pBody, MAX_SIZE))
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, pBody);
				rData.setData("controller", temp);
				nStrLen = strlen(temp);
				++nStrLen;
				pBody += nStrLen;
			}
			break;
		case initial_request:
			nType = ntohl(*((int*) pBody));
			rData.setData("type", ConvertToString(nType));
			pBody += 4;
			break;
		case sign_up_request:
		case authentication_request:
		case access_log_request:
			nType = ntohl(*((int*) pBody));
			rData.setData("type", ConvertToString(nType));
			pBody += 4;
			if (isValidStr((const char*) pBody, MAX_SIZE))
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, pBody);
				rData.setData("data", temp);
				nStrLen = strlen(temp);
				++nStrLen;
				pBody += nStrLen;
			}
			break;
		case rdm_login_request:
		case rdm_operate_request:
		case rdm_logout_request:
			if (isValidStr((const char*) pBody, MAX_SIZE))
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, pBody);
				rData.setData("data", temp);
				nStrLen = strlen(temp);
				++nStrLen;
				pBody += nStrLen;
			}
			break;
		}
	}
	else
	{
		_log("[CMP Parser]CMP body length error");
		nRet = -1;
	}

	if (-1 == nRet)
	{
		_log("[CMP Parser] parse CMP body fail");
	}
	else
	{
		nRet = rData.size();
	}

	return nRet;
}

int CCmpHandler::parseBody(const void *pData, vector<string> &vData)
{
	char * pch;
	char * pBody = (char*) ((char *) const_cast<void*>(pData) + sizeof(CMP_HEADER));

	pch = strtok(pBody, " ");
	while (pch != NULL)
	{
		vData.push_back(string(pch));
		pch = strtok( NULL, " ");
	}

	return vData.size();
}

bool CCmpHandler::isAckPacket(int nCommand)
{
	unsigned int nCommand_resp;

	nCommand_resp = generic_nack & nCommand;

	if ( generic_nack == nCommand_resp)
		return true;
	else
		return false;
}

