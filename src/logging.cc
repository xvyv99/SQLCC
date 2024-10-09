#include "logging.h"
#include <iostream>
#include <string>

logging::logging(void) {

};

logging::~logging() {

}

void logging::LogMsg(char* file_name, int64_t line_num, enum LogLevel level, char* msg) {
    std::cout<<level<<' '<<file_name<<':'<<line_num<<'] '<<msg<<std::endl;
}