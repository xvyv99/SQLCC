#include <string>
#include <sqlite3.h>
#include <optional>
#include <utility>
#include <vector>
#include <cstddef>
#include <memory>
#include "dataframe.hpp"

#ifndef _SQL_H
#define _SQL_H

namespace SQL {

class Error {
public:
	enum class Code {
		OK, // 操作成功
		ERROR, // 通用错误
		INTERNAL, // SQL 内部逻辑错误
		PERM, // 访问被拒绝
		ABORT, // 操作被中止
		BUSY, // 数据库文件被其他连接占用
		LOCKED, // 数据库被锁定
		NOMEM, // 内存不足
		READONLY, // 数据库是只读的
		INTERRUPT, // 操作被中断
		UNKNOWN,
	};

	virtual bool is(Code)=0;
protected:
	enum Code rc_;
	int rc_orig_;
};

class SQLiteError: public Error {
public:
	SQLiteError(int);
	~SQLiteError(void);

	bool is(Error::Code) override;
};

// using Result = DataFrame<std::string>;
using col_name = std::vector<std::string>;
using ret_line = std::optional<std::vector<std::string_view>>;
using ret_str = std::optional<std::string_view>;

class Result: public DataFrame<std::string> {
private:
	std::unique_ptr<Error> err_; // 传递查询状态 // TODO:计划改成枚举形式，以兼容多种 SQL 数据库
	// 使用智能指针的原因是为了调用抽象类 Error 的 is 方法
public:
	Result (std::unique_ptr<Error> err) : DataFrame<std::string>(), err_(std::move(err)) {};

	bool isSuccess(void) {
		return this->err_->is(Error::Code::OK);
	};
};

class Stmt {
protected:
	// sql.. stmt_;
	// ..Conn conn_;
	std::unique_ptr<Error> err_; // 传递查询状态 // TODO:计划改成枚举形式，以兼容多种 SQL 数据库
public:
	bool isSuccess(void) const {
		return (this->err_->is(Error::Code::OK));
	}

	virtual bool fmt(std::vector<std::string_view>)=0; // TODO: 所有需要绑定的值均视为TEXT,需改进

	virtual int step(void)=0; 

	virtual int colCount(void)=0;
	virtual ret_str colName(void)=0;
	virtual ret_line colNames(void)=0;
	
	virtual ret_str get_TEXT()=0;
	virtual ret_line getRow_TEXT()=0;
};

class Conn {
protected:
	// sql.. db_;
public:
	friend class Stmt;

	enum class ColType {
        TEXT, INTEGER, REAL, BLOB
    };

    virtual Result exec(std::string)=0;
	virtual std::unique_ptr<Stmt> preCompile(std::string_view)=0;

	bool create(
		std::string, std::vector<std::string>, 
		std::vector<Conn::ColType>, bool
	);

    Result selectAll(std::string);

    Result select(
		std::string, std::vector<std::string>, std::string
	);

	Result select(
		std::string, std::vector<std::string>
	);

	bool insert(void);
	bool update(void);
	bool del(void);

	Result getTables(void);
private:
	static std::string getTypeName(ColType);
};

class SQLiteConn: public Conn {
private:
	sqlite3 *db_;
public:
	friend class SQLiteStmt;

	SQLiteConn(std::string_view);
	~SQLiteConn();

	Result exec(std::string) override;
	std::unique_ptr<Stmt> preCompile(std::string_view) override;
};

class SQLiteStmt: public Stmt {
private:
	sqlite3_stmt *stmt_;
	sqlite3* db_;
public:
	SQLiteStmt(sqlite3*, std::string_view);
	~SQLiteStmt(void);
	
	bool fmt(std::vector<std::string_view>) override; 

	int step(void) override; 

	int colCount(void) override;
	ret_str colName(void) override;
	ret_line colNames(void) override;
	
	ret_str get_TEXT() override;
	ret_line getRow_TEXT() override;
};

} // namespace SQL

#endif
