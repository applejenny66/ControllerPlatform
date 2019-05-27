/*
 * CChihlee.h
 *
 *  Created on: 2019年3月15日
 *      Author: jugo
 */

#pragma once

#include <map>
#include <list>

class JSONObject;
class CMysqlHandler;


class CChihlee
{
public:
	explicit CChihlee();
	virtual ~CChihlee();
	void runAnalysis(const char *szInput, JSONObject &jsonResp);
	void init();

private:
	void playSound(const char *szWav);
	void intent(int nIntent, int nType, const char* szWord,JSONObject &jsonResp);
	std::string course(int nType, const char* szWord);
	std::string office(int nType, const char* szWord);


private:
	CMysqlHandler *mysql;
	std::list<std::map<std::string, std::string> > listKeyWord;

};
