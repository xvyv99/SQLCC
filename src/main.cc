#include <cstdio>
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "sql.h"

int main(int argc, char** argv){
    if (argc!=3) {
        fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return 1;
    }

    SQLhandle test = SQLhandle(std::string(argv[1]));
    
    auto r = test.exec(argv[2]);
    std::cout<<r.value()<<std::endl;

    printf("Test message");
    return 0;
}