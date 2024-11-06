#include <string>
#include <sqlite3.h>
#include <optional>
#include <utility>
#include <vector>
#include <cstddef>
#include <expected>
#include <memory>
#include "dataframe.hpp"

#ifndef _SQL_H
#define _SQL_H

namespace SQL {

enum class Error {
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

	PARAM_ERROR, // 参数错误
};

template <class T>
using Err=std::expected<T, Error>;
template <class T>
using Err_ptr=std::expected<std::unique_ptr<T>, Error>;

using Result = DataFrame<std::string>;
using col_name = std::vector<std::string>;
using ret_line = std::optional<std::vector<std::string>>;
using ret_str = std::optional<std::string>;

/*class Result: public DataFrame<std::string> {
public:
	Result () : DataFrame<std::string>() {};
};*/

class Stmt {
public:
	virtual Error fmt(std::vector<std::string_view>)=0; // TODO: 所有需要绑定的值均视为TEXT,需改进

	virtual Err<bool> step(void)=0; 

	virtual unsigned int colCount(void)=0;
	virtual std::vector<std::string> colNames(void)=0;
	
	virtual Err<ret_str> get_TEXT()=0;
	virtual std::vector<std::string> getRow_TEXT()=0;

	virtual Err_ptr<Result> query()=0;
};

class Conn {
protected:
	// sql.. db_;
public:
	friend class Stmt;

	enum class ColType {
        TEXT, INTEGER, REAL, BLOB
    };

    virtual Err_ptr<Result> exec(std::string)=0;
	virtual std::unique_ptr<Stmt> preCompile(std::string_view)=0;

	Error create(
		std::string, std::vector<std::string>, 
		std::vector<Conn::ColType>, bool
	);

    Err<std::unique_ptr<Result>> selectAll(std::string);

    Err<std::unique_ptr<Result>> select(
		std::string, std::vector<std::string>, std::string
	);

	Err<std::unique_ptr<Result>> select(
		std::string, std::vector<std::string>
	);

	bool insert(void);
	bool update(void);
	bool del(void);

	Err_ptr<Result> getTables(void);
private:
	static std::string getTypeName(ColType);
};

} // namespace SQL

#endif
