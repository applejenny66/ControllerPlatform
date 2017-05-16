#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "common.h"
#include "packet.h"
#include "CApplication.h"

using namespace std;

class CClientMeetingAgent;
class CConfig;
class CThreadHandler;

class CController: public CApplication
{
public:
	explicit CController();
	virtual ~CController();

protected:
	// return message queue key here
	int onCreated(void* nMsqKey);

	// allocate resources here
	int onInitial(void* szConfPath);

	// release resources here
	int onFinish(void* nMsqKey);

	void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);
	virtual void onHandleMessage(Message &message);
private:
	int mnMsqKey; 
	std::unique_ptr<CClientMeetingAgent> mCClientMeetingAgent;

	std::unique_ptr<CThreadHandler> tdEnquireLink;
	std::unique_ptr<CThreadHandler> tdExportLog;
	std::vector<int> vEnquireLink;

	int startClientMeetingAgent(string strIP, const int nPort, const int nMsqKey);
};
