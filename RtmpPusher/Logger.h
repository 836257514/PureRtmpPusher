#pragma once
#include "LogEngine.h"

class Logger
{
private:
	static LogEngine* m_logEngine;
public:
	static void write(const char* content);
	static void write(const char* content, int ret);
	static void init(const char* directory);
};

