#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <iterator>
#include <utility>
#include <optional>
#include <expected>
#include <exception>
#include <sstream>
#include <memory>

#include <sqlite3.h>
#include "sql.h"
#include "sqlite.h"

namespace SQL {

Error SQLiteErrorTranform(int err_code) {
	Error result;
	switch (err_code) {
	case SQLITE_OK:
		result = Error::OK;
		break;
	default:
		result = Error::UNKNOWN;
		break;
	}
	return result;
}

std::unique_ptr<Stmt> SQLiteConn::preCompile(std::string_view stmt) {
	std::unique_ptr<SQLiteStmt> sqlite_stmt = std::make_unique<SQLiteStmt>(this->db_, stmt);
	return std::move(sqlite_stmt);
}

SQLiteConn::SQLiteConn(std::string_view db_path) {
    int64_t rc = sqlite3_open(db_path.data(), &(this->db_));
    if (rc) {
		std::cout<<"Can't open database {}!"<<sqlite3_errmsg(this->db_)<<std::endl;
        sqlite3_close(this->db_);
        exit(1);
    }
}

SQLiteConn::~SQLiteConn() {
    sqlite3_close(this->db_);
}

// FIXME:所有的列全视为 TEXT,需要改进
Err_ptr<Result> SQLiteConn::exec(std::string stmt) {
	Err_ptr<Result> res;

	sqlite3_stmt* sql_stmt;
	int rc = sqlite3_prepare_v2(this->db_, stmt.c_str(), stmt.length(), &sql_stmt, NULL);
	if (rc==SQLITE_OK) {
		std::unique_ptr<Result> sqlite_res = std::make_unique<Result>();
		while (sqlite3_step(sql_stmt)==SQLITE_ROW) {
			int col_count = sqlite3_column_count(sql_stmt);
			if (!sqlite_res->hasValue()) {
				std::vector<std::string> col_names;
				for (int i=0;i<col_count;i++) {
					col_names.emplace_back(sqlite3_column_name(sql_stmt, i));
				}
				sqlite_res->setColN(col_names);
			}
			std::vector<std::string> row; row.reserve(col_count); //预留指定大小,防止多次分配
			for (int i=0;i<col_count;i++) {
				const unsigned char* text_value = sqlite3_column_text(sql_stmt, i);
				if (text_value) {
                	row.push_back(reinterpret_cast<const char*>(text_value));  
            	} else {
                	row.push_back("");  
            	}
			}
			sqlite_res->addRow(row);
		}
		res = std::move(sqlite_res); // FIXME: 需完成到智能指针的转换
	} else {
		std::cout<<"SQL error {}!"<<sqlite3_errmsg(this->db_)<<std::endl;
		res = std::unexpected(SQLiteErrorTranform(rc));
	}

	sqlite3_finalize(sql_stmt);
	return res;
}

SQLiteStmt::SQLiteStmt(sqlite3* db, std::string_view sql_stmt) : db_(db) {
	int rc_ = sqlite3_prepare_v2(db_, sql_stmt.data(), sql_stmt.length(), &this->stmt_, NULL);
	this->err_ = SQLiteErrorTranform(rc_);
}

SQLiteStmt::~SQLiteStmt() {}

Error SQLiteStmt::fmt(std::vector<std::string_view> params) {
	int bind_param_count = sqlite3_bind_parameter_count(this->stmt_);
	int count = std::min(static_cast<size_t>(bind_param_count), params.size());
	for (int i=0;i<count;i++) {
		sqlite3_bind_text(this->stmt_, i+1, params[i].data(), params[i].length(), SQLITE_TRANSIENT);
	}
	return Error::OK;
} // FIXME: 目前只会返回 Error::OK 

Error SQLiteStmt::step(void) {return Error::OK;} // TODO

Err_ptr<Result> SQLiteStmt::query() {return std::make_unique<Result>();};

Err<int> SQLiteStmt::colCount(void) {return std::unexpected(Error::OK);} // TODO
Err<ret_str> SQLiteStmt::colName(void) {return std::unexpected(Error::OK);} // TODO
Err<ret_line> SQLiteStmt::colNames(void) {return std::unexpected(Error::OK);} // TODO

Err<ret_str> SQLiteStmt::get_TEXT() {return std::unexpected(Error::OK);} // TODO
Err<ret_line> SQLiteStmt::getRow_TEXT() {return std::unexpected(Error::OK);} // TODO

}