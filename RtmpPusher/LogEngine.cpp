#include "LogEngine.h"
#include <io.h>
#include <time.h>
#include <string.h>
#include <direct.h>
#pragma warning(disable: 4996)
using namespace std;

LogEngine* LogEngine::m_instance = NULL; //定义及初始化

LogEngine::LogEngine()
{
	m_file = NULL;
	memset(m_currentLogFileName, 0x00, 50);
}

LogEngine::~LogEngine()
{
	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

LogEngine* LogEngine::get_instance()
{
	if (NULL != m_instance)
	{
		return m_instance;
	}
	else
	{
		m_instance = new LogEngine();
		return m_instance;
	}
}


bool LogEngine::init_logger(const char* directory)
{
	if (directory)
	{
		if (_access(directory, 0) == -1)
		{
#ifdef _WIN32
			_mkdir(directory);
#endif
		}

		char localTime[50] = { 0 };
		time_t t = time(0);
		strftime(localTime, 50, "%Y_%m_%d_%H_%M", localtime(&t));
		sprintf(m_currentLogFileName, "%s/%s.txt", directory, localTime);
		m_file = fopen(m_currentLogFileName, "w+");
		return m_file != NULL;
	}
}

void LogEngine::get_local_time()
{
	time_t t = time(0);
	strftime(m_timeString, 50, "%Y/%m/%d %X", localtime(&t));
}

void LogEngine::write(const char* content)
{
	get_local_time();
	size_t contentLength = strlen(content);
	char* temp = new char[50 + contentLength];
	if (content[contentLength - 1] == '\n')
	{
		sprintf(temp, "%s : %s", m_timeString, content);
	}
	else
	{
		sprintf(temp, "%s : %s\n", m_timeString, content);
	}

	printf(temp);
	fputs(temp, m_file);
	fflush(m_file);
	delete[] temp;
}

void LogEngine::write(const char* content, int ret)
{
	get_local_time();
	size_t contentLength = strlen(content);
	char* temp = new char[100 + contentLength];
	if (content[contentLength - 1] == '\n')
	{
		sprintf(temp, "%s : %s ret code : %d", m_timeString, content, ret);
	}
	else
	{
		sprintf(temp, "%s : %s ret code : %d\n", m_timeString, content, ret);
	}

	printf(temp);
	fputs(temp, m_file);
	fflush(m_file);
	delete[] temp;
}
