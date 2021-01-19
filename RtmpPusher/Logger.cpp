#include "Logger.h"

LogEngine* Logger::m_logEngine = NULL;

void Logger::write(const char* content)
{
	m_logEngine->write(content);
}

void Logger::write(const char* content, int ret)
{
	m_logEngine->write(content, ret);
}

void Logger::init(const char* directory)
{
	if (m_logEngine == NULL)
	{
		m_logEngine = LogEngine::get_instance();
	}

	m_logEngine->init_logger(directory);
}