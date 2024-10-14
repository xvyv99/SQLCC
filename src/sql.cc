#include <iostream>
#include <cstdint>
#include <string>
#include <optional>
#include "logging.h"

#include <sqlite3.h>
#include "sql.h"

SQLhandle::SQLhandle(std::string db_name) {
    int64_t rc = sqlite3_open(db_name.c_str(), &(this->db_));
    if (rc) {
        fprintf(stderr, "Can't open database %s!", sqlite3_errmsg(this->db_));
        sqlite3_close(this->db_);
        exit(1);
    }
}

SQLhandle::~SQLhandle() {
    sqlite3_close(this->db_);
}

std::optional<std::string> SQLhandle::exec(std::string stmt) {
    char *zErrMsg = NULL;
    int64_t rc = rc = sqlite3_exec(this->db_, stmt.c_str(), CallBack, 0, &zErrMsg);
    if (rc!=SQLITE_OK) {
        fprintf(stderr, "SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return std::nullopt;
    }
    return std::make_optional("Success!");
}

/**
 * @brief 回调函数,用于打印结果SQL查询结果,来源于 [SQLite In 5 Minutes Or Less](https://sqlite.org/quickstart.html).
 */
int CallBack(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int64_t i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
