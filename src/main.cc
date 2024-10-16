#include <string>
#include <sqlite3.h>
#include "sql.h"
#include "spdlog/spdlog.h"

int main(int argc, char** argv){
    if (argc!=3) {
        fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return 1;
    }

    SQLhandle test{std::string(argv[1])};
    
    auto r = test.exec(argv[2]);
	spdlog::info(r.value_or("Failed"));

	spdlog::info("Test message.");
    return 0;
}
