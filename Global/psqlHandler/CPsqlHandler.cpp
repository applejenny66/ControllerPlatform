/*
 * CPsqlHandler.cpp
 *
 *  Created on: 2017年1月5日
 *      Author: jugo
 */

#include "CPsqlHandler.h"
#include "LogHandler.h"
#include <libpq-fe.h>

PGconn *conn = 0;

CPsqlHandler::CPsqlHandler()
{
	conn = 0;
}

CPsqlHandler::~CPsqlHandler()
{
	close();
}

int CPsqlHandler::open(const char *pghost, const char *pgport, const char *dbName, const char *login, const char *pwd)
{
	close();
	conn = PQsetdbLogin(pghost, pgport, NULL, NULL, dbName, login, pwd);

	//ConnStatusType的值最常用的两个是CONNECTION_OK或 CONNECTION_BAD。
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("[CPsqlHandler] Connection to database failed: %s", PQerrorMessage(conn));
		close();
		return 0;
	}

	printf("[CPsqlHandler] Connection to database Success\n");
//	printf("User: %s\n", PQuser(conn));
//	printf("Database name: %s\n", PQdb(conn));
//	printf("Password: %s\n", PQpass(conn));
//	printf("server_version: %s\n", PQparameterStatus(conn, "server_version"));
//	printf("server_encoding: %s\n", PQparameterStatus(conn, "server_encoding"));
//	printf("client_encoding: %s\n", PQparameterStatus(conn, "client_encoding"));
//	printf("session_authorization: %s\n", PQparameterStatus(conn, "session_authorization"));
//	printf("DateStyle: %s\n", PQparameterStatus(conn, "DateStyle"));
//	printf("TimeZone: %s\n", PQparameterStatus(conn, "TimeZone"));
//	printf("integer_datetimes: %s\n", PQparameterStatus(conn, "integer_datetimes"));
//	printf("standard_conforming_strings: %s\n", PQparameterStatus(conn, "standard_conforming_strings"));

	return 1;
}

void CPsqlHandler::close()
{
	if (0 != conn)
		PQfinish(conn);
	conn = 0;
	_log("[CPsqlHandler] PQfinish");
}

int CPsqlHandler::sqlExec(const char *szSQL)
{
	int nRet = ERROR_SUCCESS;

	if (0 == conn)
	{
		printf("[CPsqlHandler] Invalid PGconn");
		return ERROR_INVALID_CONN;
	}
	PGresult *res = PQexec(conn, szSQL);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		// already exists , not show error
		string strError = PQerrorMessage(conn);
		if (0 >= strError.find("already exists"))
		{
			_log("[CPsqlHandler] sqlExec Fail: %s\rSQL:%s", PQerrorMessage(conn), szSQL);
			nRet = ERROR_FAIL_EXECSQL;
		}
	}
	else
	{
		_log("[CPsqlHandler] sqlExec Success: %s", szSQL);
	}
	PQclear(res);
	return nRet;
}

int CPsqlHandler::sqlExec(list<string> listSQL)
{
	if (0 == conn)
	{
		printf("[CPsqlHandler] Invalid PGconn");
		return ERROR_INVALID_CONN;
	}

	PGresult *res = PQexec(conn, "BEGIN");

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("BEGIN command failed\n");
		PQclear(res);
		return ERROR_FAIL_BEGIN;
	}

	string strItem;
	for (list<string>::iterator it = listSQL.begin(); it != listSQL.end(); ++it)
	{
		strItem = (*it);
		if (!strItem.empty())
		{
			res = PQexec(conn, strItem.c_str());

			if (PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				printf("SQL Exec failed: %s\n", PQerrorMessage(conn));
				PQclear(res);
			}
		}
	}

	res = PQexec(conn, "COMMIT");

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("COMMIT command failed\n");
		PQclear(res);
		return ERROR_FAIL_COMMIT;
	}

	PQclear(res);

	return ERROR_SUCCESS;
}

int CPsqlHandler::query(const char *szSQL, list<map<string, string> > &listRest)
{
	if (0 == conn)
	{
		printf("[CPsqlHandler] Invalid PGconn");
		return ERROR_INVALID_CONN;
	}

	PGresult *res = PQexec(conn, szSQL);

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{

		printf("[CPsqlHandler] No data retrieved :%s\n", szSQL);
		PQclear(res);
		return 0;
	}

	int rows = PQntuples(res);

	if (0 < rows)
	{
		map<string, string> mapValue;
		int nFields = PQnfields(res);
		for (int i = 0; i < PQntuples(res); ++i)
		{
			mapValue.clear();
			for (int j = 0; j < nFields; ++j)
			{
				mapValue[PQfname(res, j)] = PQgetvalue(res, i, j);
			}
			listRest.push_back(mapValue);
		}
		mapValue.clear();
	}

	PQclear(res);
	return listRest.size();
}
