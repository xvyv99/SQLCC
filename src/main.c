#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;   
}

int main(int argc, char** argv){
    sqlite3 *db;
    char *zErrMsg = NULL;
    int rc;

    if (argc!=3) {
        fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return 1;
    }

    rc = sqlite3_open(argv[1], &db);
    if (rc) {
        fprintf(stderr, "Can't open %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
    if (rc!=SQLITE_OK) {
        fprintf(stderr, "SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    printf("Test message");
    return 0;
}