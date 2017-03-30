/*
 * main.cpp
 *
 *  Created on: 2016年07月01日
 *      Author: Jugo
 */

#include <unistd.h>
#include <string.h>
#include <iostream>

#include "CController.h"
#include "CProcessHandler.h"
#include "CMessageHandler.h"
#include "event.h"
#include "LogHandler.h"
#include "CConfig.h"
#include "utility.h"
#include "common.h"

using namespace std;

string getConfName(string strProcessName);
void options(int argc, char **argv);
static void runService();

int main(int argc, char* argv[])
{
	extern char *__progname;
	openlog(__progname, LOG_PID, LOG_LOCAL0);

	// Run Process
	CProcessHandler::runProcess(runService);

	closelog();
	return EXIT_SUCCESS;
}

string getConfName(std::string strProcessName)
{
	size_t found = strProcessName.find_last_of("/\\");
	return (strProcessName.substr(++found) + ".conf");
}

void runService()
{
	int nInit = TRUE;
	int nTmp = -1;
	int nMsgID = EVENT_MSQ_KEY_CONTROLLER_TRACKER;
	extern char *__progname;

	CController *controller = CController::getInstance();
	CConfig *config = new CConfig();
	string *pstrConf = new string(getConfName(__progname));

	_log("Get Config File : %s", pstrConf->c_str());
	if (FALSE != config->loadConfig(*pstrConf))
	{
		_setLogPath(config->getValue("LOG", "log").c_str());

		if (controller->initMessage(nMsgID))
		{
			if (0 == config->getValue("MONGODB", "enable").compare("yes"))
			{
				convertFromString(nTmp, config->getValue("MONGODB", "port"));

				if (!controller->startClientMongoDB(config->getValue("MONGODB", "ip"), nTmp, nMsgID))
				{
					nInit = FALSE;
					_log("[Controller] Create Client Controller-MONGODB Service Fail. Port : %d , Message ID : %d",
							nTmp, nMsgID);
				}
				else
				{
					_log("[Controller] Create Client Controller-MONGODB Service Success. Port : %d , Message ID : %d",
							nTmp, nMsgID);
				}
			}

			if (0 == config->getValue("SERVER DEVICE", "enable").compare("yes"))
			{
				convertFromString(nTmp, config->getValue("SERVER DEVICE", "port"));
				if (!controller->startServerAccesslog(config->getValue("SERVER DEVICE", "ip"), nTmp))
				{
					nInit = FALSE;
					_log("[Controller] Create Server DEVICE Service Fail. Port : %d ", nTmp);
				}
				else
				{
				_log("[Controller] Create Server DEVICE Service Success. Port : %d ", nTmp);

			}
		}
	}
	else
	{
		nInit = FALSE;
		_log("[Controller] Create Message Queue Fail");
	}
}
else
{
	nInit = FALSE;
	_log("[Controller] Load Configuration File Fail");
}
delete pstrConf;
delete config;

if (TRUE == nInit)
{
	cout << "\n<============= (◕‿‿◕｡) ... Service Start Run ... ԅ(¯﹃¯ԅ) =============>\n" << endl;
	controller->run(EVENT_FILTER_CONTROLLER, "Controller");

	cout << "\n<============= ( #｀Д´) ... Service Stop Run ... (╬ ಠ 益ಠ) =============>\n" << endl;
	controller->stopServerAccesslog();
	CMessageHandler::release();
}
_close();
delete controller;
}
/**
 * process options
 */
void options(int argc, char **argv)
{
int c;

while ((c = getopt(argc, argv, "M:P:F:m:p:f:H:h")) != -1)
{
	switch (c)
	{
	case 'H':
	case 'h':
		printf("this is help\n");
		break;
	}
}
}

