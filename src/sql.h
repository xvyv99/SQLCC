#include <string>
#include <sqlite3.h>
#include <optional>

#ifndef _SQL_H
#define _SQL_H

int CallBack(void *NotUsed, int argc, char **argv, char **azColName);

class SQLhandle {
private:
    sqlite3 *db_;
public:
    SQLhandle(std::string db_name);
    ~SQLhandle();

    std::optional<std::string> exec(std::string stmt);
};

#endif