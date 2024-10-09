#include <string>
#include <cstdint>

#ifndef _LOGGING_H
#define _LOGGING_H

#define LOG(LEVEL, MSG) logging::LogMsg(__FILE__, __LINE__, (LEVEL), MSG);

enum LogLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class logging {
private:
public:
    logging(void);
    ~logging();
    static void LogMsg(char* file_name, int64_t line_num, enum LogLevel level, char* msg);
};

#endif