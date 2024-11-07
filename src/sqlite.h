#include "sql.h"

#ifndef _SQLITE_H
#define _SQLITE_H

namespace SQL {

int SQLiteCallBack(void*, int, char**, char**);
Error SQLiteErrorTranform(int);

class SQLiteConn: public Conn {
private:
	
public:
	sqlite3 *db_;
	friend class SQLiteStmt;

	SQLiteConn(std::string_view);
	~SQLiteConn();

	std::optional<std::string> errMsg(void);

	Err_ptr<Result> exec(std::string) override;
	Err_ptr<Result> exec_v1(std::string);
	Err_ptr<Stmt> preCompile(std::string_view) override;
};

class SQLiteStmt: public Stmt {
private:
	sqlite3_stmt *stmt_;
	int rc_;
public:

	friend class SQLiteConn;
	SQLiteStmt(sqlite3*, std::string_view);
	~SQLiteStmt(void);
	
	bool fmt(std::vector<std::string_view>) override;
	void fmt_loose(std::vector<std::string_view>) override;

	Err<bool> step(void) override;

	unsigned int colCount(void) override;
	std::vector<std::string> colNames(void) override;
	
	Err<ret_str> get_TEXT() override;
	std::vector<std::string> getRow_TEXT() override;

	Err_ptr<Result> query() override;
};

}

#endif