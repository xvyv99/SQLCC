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
	//virtual Stmt preCompile(std::string)=0;

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
	//Stmt preCompile(std::string) override;
};

class SQLiteStmt: public Stmt {
private:
	sqlite3_stmt *stmt_;
	sqlite3* db_;

	SQLiteStmt(sqlite3* db, std::string_view sql_stmt) : db_(db) {
		int rc_ = sqlite3_prepare_v2(db_, sql_stmt.data(), sql_stmt.length(), &this->stmt_, NULL);
		std::unique_ptr<SQLiteError> sqlite_err = std::make_unique<SQLiteError>(rc_);
		this->err_ = std::move(sqlite_err);
	}
	~SQLiteStmt(void);
public:
	bool fmt(std::vector<std::string_view>); // TODO: 所有需要绑定的值均视为TEXT,需改进

	int step(void); // TODO: 计划将状态码变得更加类型安全

	int colCount(void);
	ret_str colName(void);
	ret_line colNames(void);
	
	ret_str get_TEXT();
	ret_line getRow_TEXT();
};

} // namespace SQL

#endif