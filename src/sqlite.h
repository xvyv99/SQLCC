#include "sql.h"

#ifndef _SQLITE_H
#define _SQLITE_H

namespace SQL {

Error SQLiteErrorTranform(int);

class SQLiteConn: public Conn {
private:
	sqlite3 *db_;
public:
	friend class SQLiteStmt;

	SQLiteConn(std::string_view);
	~SQLiteConn();

	Err_ptr<Result> exec(std::string) override;
	std::unique_ptr<Stmt> preCompile(std::string_view) override;
};

class SQLiteStmt: public Stmt {
private:
	sqlite3_stmt *stmt_;
	sqlite3* db_;
	Error err_;
public:
	SQLiteStmt(sqlite3*, std::string_view);
	~SQLiteStmt(void);
	
	Error fmt(std::vector<std::string_view>) override; 

	Error step(void) override;

	Err<int> colCount(void) override;
	Err<ret_str> colName(void) override;
	Err<ret_line> colNames(void) override;
	
	Err<ret_str> get_TEXT() override;
	Err<ret_line> getRow_TEXT() override;

	Err_ptr<Result> query() override;
};

}

#endif