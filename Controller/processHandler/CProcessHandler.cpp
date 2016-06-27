/*
 * CProcessHandler.cpp
 *
 *  Created on: 2016年6月27日
 *      Author: Jugo
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <event.h>

#include "CProcessHandler.h"
#include "CObject.h"
#include "common.h"

volatile int flag = 0;
pid_t child_pid = -1; //Global

/**
 * Child signal handler
 */
void CSigHander(int signo)
{
	_DBG("[Signal] Child Received signal %d", signo);
	flag = 1;
}

/**
 * Parent signal handler
 */
void PSigHander(int signo)
{
	if ( SIGHUP == signo)
		return;
	_DBG("[Signal] Parent Received signal %d", signo);
	flag = 1;
	sleep(3);
	kill(child_pid, SIGKILL);
}

CProcessHandler::CProcessHandler()
{

}

CProcessHandler::~CProcessHandler()
{

}

int CProcessHandler::runProcess(CObject *pObj)
{
	if (0 == pObj)
		return -1;

	pid_t w;
	int status;

	do
	{
		child_pid = fork();
		if (-1 == child_pid)
		{
			exit(EXIT_FAILURE);
		}

		if (child_pid == 0)
		{
			/**
			 * Child process
			 */
			signal(SIGINT, CSigHander);
			signal(SIGTERM, CSigHander);
			signal(SIGPIPE, SIG_IGN);
			pObj->run( EVENT_FILTER_CONTROL_CENTER);
			return 0;
		}

		/**
		 * Parent process
		 */
		signal(SIGINT, PSigHander);
		signal(SIGTERM, PSigHander);
		signal(SIGHUP, PSigHander);
		signal(SIGPIPE, SIG_IGN);

		w = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);

		if (w == -1)
		{
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
		if (WIFEXITED(status))
		{
			_DBG("[Process] child exited, status=%d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			_DBG("[Process] child killed by signal %d\n", WTERMSIG(status));
		}
		else if (WIFSTOPPED(status))
		{
			_DBG("[Process] child stopped by signal %d\n", WSTOPSIG(status));
		}
		else if (WIFCONTINUED(status))
		{
			_DBG("[Process] continued\n");
		}
		else
		{
			_DBG("[Process] receive signal: %d\n", status);
		}
		sleep(3);
	}
	while (SIGTERM != WTERMSIG(status) && !flag);

	closelog();
	exit(EXIT_SUCCESS);
	return 1;
}

