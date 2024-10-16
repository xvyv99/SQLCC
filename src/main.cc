#include <string>

#include "sql.h"
#include "spdlog/spdlog.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv){
	CLI::App app{"App description"};

	std::string filename = "default";
    app.add_option("-f,--file", filename, "A help string");

    CLI11_PARSE(app, argc, argv);

	spdlog::info("Test message.");
    return 0;
}
