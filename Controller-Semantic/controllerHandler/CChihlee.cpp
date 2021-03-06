/*
 * CChihlee.cpp
 *
 *  Created on: 2019年3月15日
 *      Author: jugo
 */

#include <string>
#include<fstream>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "JSONObject.h"
#include "CChihlee.h"
#include "CString.h"
#include "LogHandler.h"
#include "CResponsePacket.h"
#include "CFileHandler.h"
#include "CMysqlHandler.h"
#include "common.h"

using namespace std;

CChihlee::CChihlee() :
		m_strMySQLIP("127.0.0.1"),mysql(new CMysqlHandler())
{

}

CChihlee::~CChihlee()
{
	delete mysql;
}

void CChihlee::init()
{
	if (TRUE == mysql->connect(m_strMySQLIP.c_str(), "chihlee", "chihlee", "Chihlee123!", "5"))
	{
		_log("[CChihlee] init: Mysql Connect Success");
		listKeyWord.clear();
		mysql->query("select intent_id,type,word from keyWord", listKeyWord);

		string strField;
		string strValue;
		map<string, string> mapItem;
		int nCount = 0;
		for (list<map<string, string> >::iterator i = listKeyWord.begin(); i != listKeyWord.end(); ++i, ++nCount)
		{
			mapItem = *i;
			for (map<string, string>::iterator j = mapItem.begin(); j != mapItem.end(); ++j)
			{
				printf("%s : %s\n", (*j).first.c_str(), (*j).second.c_str());
			}
		}

		mysql->close();
	}
}
void CChihlee::runAnalysis(const char *szInput, JSONObject &jsonResp)
{
	CFileHandler file;
	CString strWord = szInput;
	CResponsePacket respPacket;
	CString strText;
	CString strSound;
	CString strScreen;

	strWord.replace("笑訊", "校訓");
	strWord.replace("校去", "校訓");
	strWord.replace("治理", "致理");
	strWord.trim();
	if (TRUE == mysql->connect(m_strMySQLIP.c_str(), "chihlee", "chihlee", "Chihlee123!", "5")
			&& 0 < listKeyWord.size())
	{
		_log("[CChihlee] runAnalysis: Mysql Connect Success");

		int nIntent = -1;
		int nType = 0;
		string strField;
		string strValue;
		map<string, string> mapItem;
		int nCount = 0;
		// key word 字典檔查關鍵字
		for (list<map<string, string> >::iterator i = listKeyWord.begin(); i != listKeyWord.end(); ++i, ++nCount)
		{
			mapItem = *i;
			for (map<string, string>::iterator j = mapItem.begin(); j != mapItem.end(); ++j)
			{
				printf("%s : %s\n", (*j).first.c_str(), (*j).second.c_str());
				if ((*j).first.compare("word") == 0)
				{
					if (0 <= strWord.find(j->second.c_str()))
					{
						printf("word march intent_id: %s type: %s word: %s\n", mapItem["intent_id"].c_str(),
								mapItem["type"].c_str(), (*j).second.c_str());
						nIntent = atoi(mapItem["intent_id"].c_str());
						nType = atoi(mapItem["type"].c_str());
						intent(nIntent, nType, (*j).second.c_str(), jsonResp);
						mysql->close();
						return;
					}
				}
			}
		}
		mysql->close();
	}


	remove("/chihlee/jetty/webapps/chihlee/map.jpg");

	//=============== 校園導覽 =================================//
	if (0 <= strWord.find("導覽") || 0 <= strWord.find("地圖") || 0 <= strWord.find("參觀") || 0 <= strWord.find("校園"))
	{
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_1.wav";
		strScreen = "/chihlee/jetty/webapps/chihlee/img/map.jpg";
	}

	//=============== 廁所怎麼走 =================================//
	if (0 <= strWord.find("廁所") || 0 <= strWord.find("洗手間") || 0 <= strWord.find("大便") || 0 <= strWord.find("小便")
			|| 0 <= strWord.find("方便間"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/wc_map.jpg";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_2.wav";
	}

	//=============== 我想找電動輪椅充電 =================================//
	if (0 <= strWord.find("電動輪椅") || 0 <= strWord.find("輪椅充電") || 0 <= strWord.find("充電") || 0 <= strWord.find("沒電"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/wc_map.jpg";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_3.wav";
	}

	//=============== 圖書館怎麼走 =================================//
	if (0 <= strWord.find("圖書館") || 0 <= strWord.find("圖館") || 0 <= strWord.find("書館") || 0 <= strWord.find("看書"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/wc_map.jpg";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_4.wav";
	}

	//=============== 校訓 =================================//
	if (0 <= strWord.find("校訓") || 0 <= strWord.find("誠信") || 0 <= strWord.find("致理科大"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/motto.png";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_5.wav";
	}

	//=============== 吉祥物 =================================//
	if (0 <= strWord.find("吉祥物") || 0 <= strWord.find("喜鵲"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/character.jpg";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_6.wav";
	}

	//=============== 校歌 =================================//
	if (0 <= strWord.find("校歌") || 0 <= strWord.find("學生活動"))
	{
		strScreen = "/chihlee/jetty/webapps/chihlee/img/song.jpg";
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_7.wav";
	}

	//================ 謝謝你的解說=====================//
	if (0 <= strWord.find("謝謝") || 0 <= strWord.find("感謝") || 0 <= strWord.find("掰掰") || 0 <= strWord.find("拜拜")
			|| 0 <= strWord.find("謝啦") || 0 <= strWord.find("謝拉"))
	{
		remove("/chihlee/jetty/webapps/chihlee/map.jpg");
		strSound = "/chihlee/jetty/webapps/chihlee/wav/wav_8.wav";
	}

	file.copyFile(strScreen.getBuffer(), "/chihlee/jetty/webapps/chihlee/map.jpg");
	//playSound(strSound.getBuffer());

	respPacket.setActivity<int>("type", RESP_TTS).setActivity<const char*>("lang", "zh").setActivity<const char*>("tts",
			"無法找到相關的資訊").format(jsonResp);
}

void CChihlee::playSound(const char *szWav)
{
	pid_t pid;
	int status = -1;

	if (szWav)
	{
		char *arg_list[] = { const_cast<char*>("aplay"), const_cast<char*>(szWav), NULL };

		status = posix_spawn(&pid, "/usr/bin/aplay", NULL, NULL, arg_list, environ);
		if (status == 0)
		{
			_log("[CChihlee] playSound posix_spawn Child pid: %i", pid);
			if (waitpid(pid, &status, 0) != -1)
			{
				_log("[CChihlee] playSound Child exited with status %i", status);
			}
			else
			{
				_log("[CChihlee] playSound waitpid Error");
			}
		}
		else
		{
			_log("[CChihlee] playSound Error posix_spawn: %s", strerror(status));
		}
	}
}

/**
 * intent:
 * 1 : course
 * 2 : office
 */
void CChihlee::intent(int nIntent, int nType, const char* szWord, JSONObject &jsonResp)
{
	CResponsePacket respPacket;
	std::string strWord;

	// Intent search
	switch (nIntent)
	{
	case 1: // use course table
		strWord = course(nType, szWord);
		break;
	case 2: // use office table
		strWord = office(nType, szWord);
		break;
	default: // unknow intent
		strWord = "無法理解您的問題";
		break;
	}

	respPacket.setActivity<int>("type", RESP_TTS).setActivity<const char*>("lang", "zh").setActivity<const char*>("tts",
			strWord.c_str()).format(jsonResp);
}

/**
 * course:
 * 1 : 授課名稱
 * 2 : 授課老師
 * 3 : 授課地點
 */
string CChihlee::course(int nType, const char* szWord)
{
	std::string strResponse = "無法找到相關的課程資訊";
	_log("[CChihlee] course type: %d", nType);

	CString strSQL;
	std::string strDisplay;

	list<map<string, string> > listCourse;

	switch (nType)
	{
	case 1:
		strSQL.format("SELECT * FROM chihlee.course WHERE courseName like '%%%s%%';", szWord);
		break;
	case 2:
		strSQL.format("SELECT * FROM chihlee.course WHERE teacher like '%%%s%%';", szWord);
		break;
	case 3:
		strSQL.format("SELECT * FROM chihlee.course WHERE place like '%%%s%%';", szWord);
		break;
	}

	_log("[CChihlee] course SQL : %s", strSQL.getBuffer());
	mysql->query(strSQL.getBuffer(), listCourse);

	string strField;
	string strValue;
	map<string, string> mapItem;

	CString strTemplate;
	for (list<map<string, string> >::iterator i = listCourse.begin(); i != listCourse.end(); ++i)
	{
		if (i == listCourse.begin())
		{
			strResponse = "";
			strDisplay = "";
		}
		else
		{
			strDisplay += "\n";
		}
		mapItem = *i;

		strTemplate.format("%s在每週%s第%s節,由%s老師在%s授課,,", mapItem["courseName"].c_str(), mapItem["weekDay"].c_str(),
				mapItem["credit"].c_str(), mapItem["teacher"].c_str(), mapItem["place"].c_str());
		strResponse += strTemplate.toString();

		strDisplay += mapItem["courseName"].c_str();
		strDisplay += "\n由";
		strDisplay += mapItem["teacher"].c_str();
		strDisplay += "授課";
	}

	displayWord(strDisplay.c_str());
	return strResponse;
}

string CChihlee::office(int nType, const char* szWord)
{
	string strField;
	string strValue;
	map<string, string> mapItem;
	CString strSQL;
	list<map<string, string> > listOffice;
	CString strTemplate;
	std::string strResponse = "無法找到相關的地點";
	_log("[CChihlee] office type: %d", nType);

	switch (nType)
	{
	case 1:
		strSQL.format("SELECT * FROM chihlee.office WHERE officeName like '%%%s%%';", szWord);
		break;
	}

	_log("[CChihlee] office SQL : %s", strSQL.getBuffer());
	mysql->query(strSQL.getBuffer(), listOffice);

	for (list<map<string, string> >::iterator i = listOffice.begin(); i != listOffice.end(); ++i)
	{
		if (i == listOffice.begin())
			strResponse = "";
		mapItem = *i;

		strTemplate.format("看, %s在%s%s樓,,", mapItem["officeName"].c_str(), mapItem["building"].c_str(),
				mapItem["floor"].c_str());
		strResponse += strTemplate.toString();
	}

	return strResponse;
}

void CChihlee::setMySQLIP(const char * szIP)
{
	if (0 == szIP)
		return;
	m_strMySQLIP = szIP;
}

void CChihlee::displayWord(const char * szWord)
{
	_log("[CChihlee] displayWord path: %s  word: %s", m_strWordPath.c_str(), szWord);
	CString strText;
	ofstream csWordFile(m_strWordPath.c_str(), ios::trunc);
	strText.format("%s\n          \n          ", szWord);
	csWordFile << strText.getBuffer() << endl;
	csWordFile.close();
}

void CChihlee::setWordPath(const char * szPath)
{
	if (0 == szPath)
		return;
	m_strWordPath = szPath;
}

