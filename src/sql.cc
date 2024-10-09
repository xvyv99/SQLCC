#include <iostream>
#include <string>
#include <optional>
#include <sqlite3.h>
#include "sql.h"

SQLhandle::SQLhandle(std::string db_name) {
    int rc = sqlite3_open(db_name.c_str(), &(this->db_));
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
    int rc = rc = sqlite3_exec(this->db_, stmt.c_str(), CallBack, 0, &zErrMsg);
    if (rc!=SQLITE_OK) {
        fprintf(stderr, "SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return std::nullopt;
    }
    return std::make_optional("Success!");
}

int CallBack(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}