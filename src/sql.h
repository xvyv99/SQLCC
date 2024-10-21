#include <string>
#include <sqlite3.h>
#include <optional>
#include <utility>
#include <vector>
#include <cstddef>

#ifndef _SQL_H
#define _SQL_H

using sql_result = std::vector<std::vector<std::string>>; 
// 定义外层为行,内层为列,即[row][col]
using row_name = std::vector<std::string>;
using ret_line = std::optional<std::vector<std::string_view>>;
using ret_str = std::optional<std::string_view>;

class SQLResult {
protected:
	sql_result result_;
	row_name row_;
	std::size_t line_count_;
public:
	friend class SQLiteHandle;

	SQLResult(void);
	~SQLResult(void);

	int lineCount(void) const;
	std::optional<std::pair<std::size_t, std::size_t>> shape(void) const;
	bool hasValue(void) {
		return this->line_count_!=-1;
	};

	ret_str operator()(std::size_t, std::string_view) const;
	ret_line operator()(std::size_t) const;
	ret_line operator()(std::string_view) const;
};

class SQLiteResult : public SQLResult {
public:
	friend int SQLiteCallBack(SQLiteResult&, int, char **, char **);
};

int CallBack(void *NotUsed, int argc, char **argv, char **azColName);
int SQLiteCallBack(SQLiteResult&, int, char **, char **);

class SQLHandle {
public:
    virtual SQLResult exec(std::string)=0;
	SQLResult create(void);
	SQLResult insert(void);
	SQLResult update(void);
	SQLResult del(void);
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
