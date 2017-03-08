#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdexcept>
#include "LogHandler.h"

#ifdef __cplusplus
extern "C"
{

//===========Data Type ============//
#ifndef NULL
#define NULL	0
#endif

#ifndef BOOL
#define BOOL	int
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef ULONG
#define ULONG	unsigned long
#endif

#ifndef UINT
#define UINT	unsigned int
#endif

//=========== Debug ==============//
#define _DBG(msg, arg...) printf("[DEBUG] " msg"\n" , ##arg)
#define _TRACE(msg, arg...) printf("[TRACE] %s:%s(%d): " msg"\n" ,__FILE__, __FUNCTION__, __LINE__, ##arg)
#define _BREAK printf("[BREAK] %s:%s(%d): \n" ,__FILE__, __FUNCTION__, __LINE__)
#define _EXCEPTION(msg, arg...) _log("[EXCEPTION] %s:%s(%d): " msg"\n" ,__FILE__, __FUNCTION__, __LINE__, ##arg)

//=========== Function ===========//
#define BUF_SIZE		2048	// socket send & recv buffer
#define BACKLOG		128		// How many pending connections queue will hold
#define SUCCESS		1
#define	 FAIL				-1
//typedef void (*CBFun)(void* param);

//========= Exception ===========//
#define _TRY		try{
#define _CATCH		}catch(std::out_of_range &e){\
		_EXCEPTION("[Exception] Caught an out_of_range exception: %s\n",e.what());}\
	catch(std::runtime_error &e){\
		_EXCEPTION("[Exception] Caught a runtime_error exception: %s\n",e.what());}\
	catch(std::exception &e){\
		_EXCEPTION("[Exception] Caught an exception of an unexpected type: %s\n",e.what());}\
		catch (...){\
			_EXCEPTION("[Exception] Caught an unknown exception\n");}

}
;
#endif
