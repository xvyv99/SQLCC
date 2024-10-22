#include <string>
#include <sqlite3.h>
#include <optional>
#include <utility>
#include <vector>
#include <cstddef>
#include "dataframe.hpp"

#ifndef _SQL_H
#define _SQL_H

// using SQLResult = DataFrame<std::string>;
using col_name = std::vector<std::string>;
using ret_line = std::optional<std::vector<std::string_view>>;
using ret_str = std::optional<std::string_view>;

class SQLResult: public DataFrame<std::string> {
private:
	int rc_; // 传递查询状态 // TODO:计划改成枚举形式，以兼容多种 SQL 数据库
public:
	SQLResult (int rc) : DataFrame<std::string>(), rc_(rc) {};

	bool isSuccess(void) {
		return (this->rc_==SQLITE_OK);
	};
};

class SQLHandle {
public:
	enum class ColType {
        TEXT, INTEGER, REAL, BLOB
    };

    virtual SQLResult exec(std::string)=0;

	bool create(
		std::string table_name, 
		std::vector<std::string> col_names, 
		std::vector<SQLHandle::ColType> col_types,
		bool create_if_exists
	);

	SQLResult select(void);
	bool insert(void);
	bool update(void);
	bool del(void);

	SQLResult getTables(void);
private:
	static std::string getTypeName(ColType);
};

class SQLiteHandle: public SQLHandle {
private:
	sqlite3 *db_;
public:
	SQLiteHandle(std::string_view db_name);
	~SQLiteHandle();

	SQLResult exec(std::string) override;
};

#endif
