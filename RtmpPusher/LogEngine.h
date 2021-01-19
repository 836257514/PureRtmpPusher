#pragma once
#include <cstdio>
#include <string>

class LogEngine
{
private:
    static LogEngine* m_instance; //ÉêÃ÷
    FILE* m_file;
    char m_timeString[50];
    char m_currentLogFileName[50];
    LogEngine();
    inline void get_local_time();
public:
    ~LogEngine();
    static LogEngine* get_instance();
    bool init_logger(const char* directory);
    void write(const char* content);
    void write(const char* content, int ret);
};

