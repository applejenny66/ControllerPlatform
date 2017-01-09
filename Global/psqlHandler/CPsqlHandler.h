/*
 * CPsqlHandler.h
 *
 *  Created on: 2017年1月5日
 *      Author: jugo
 */

#pragma once
#include <list>
#include <map>
#include <string>
using namespace std;

typedef enum
{
	ERROR_FAIL = 0,
	ERROR_SUCCESS = 1,
	ERROR_INVALID_CONN = -1,
	ERROR_INVALID_SQL = -2,
	ERROR_FAIL_EXECSQL = -3,
	ERROR_EXCEPTION = -4,
	ERROR_FAIL_BEGIN = -5,
	ERROR_FAIL_COMMIT = -6
} PSQL_STATUS;

class CPsqlHandler
{
public:
	explicit CPsqlHandler();
	virtual ~CPsqlHandler();
	int open(const char *pghost, const char *pgport, const char *dbName, const char *login, const char *pwd);
	void close();
	int sqlExec(const char *szSQL);
	int sqlExec(list<string> listSQL);
	int query(const char *szSQL, list<map<string, string> > &listRest);

private:

};
