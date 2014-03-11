//LogM.h

//��־������
/**
* Auth: Karl
* Date: 2014/2/28
* LastUpdate: 2014/2/28
*/
#pragma once

#ifndef _LOG_M_HEADER_
#define _LOG_M_HEADER_

#include "stdio.h"

#ifndef MAX_PATH
#define MAX_PATH  512;
#endif

class LogM{

private:
	FILE *logFile;
	LogM();
	~LogM();

public:
	char EventLogFile[MAX_PATH];
	char appPath[MAX_PATH];
	char sleepTime;

	
public:
	static LogM* Instance();
	static LogM* newInstance();

	int		StartLog(int sleepTime=600);//ʱ��(��)��ÿ�� sleepTime ������һ���µ�log�ļ��� ��Χ: 10 - MAX, Ĭ�� 600��(10����)
	FILE*	OpenLog();
	void	CloseLog(FILE* fp=0);

	FILE*	GetLogFile(){ return logFile;};

};

void __cdecl log(const char *format, ...);

#endif

