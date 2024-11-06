#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>

#include <utility>
#include <optional>
#include <expected>
#include <stdexcept>
#include <sstream>
#include <memory>

#include <sqlite3.h>
#include "sql.h"
#include "sqlite.h"

namespace SQL {

int SQLiteCallBack(void *res, int argc, char **argv, char **azColName) {
	std::vector<std::string> row; row.reserve(argc);
	Result *sqlite_res = static_cast<Result*>(res);

	if (!sqlite_res->hasValue()) {
		std::vector<std::string> col_names; col_names.reserve(argc);
		for (int i=0; i<argc; i++) {
			col_names.emplace_back(std::string(azColName[i]));
		}
		sqlite_res->setColN(col_names);
	} 

    for (int i = 0; i < argc; i++) {
		row.emplace_back(argv[i]);
    }
	sqlite_res->addRow(row);

    return 0;
}

Error SQLiteErrorTranform(int err_code) {
	Error result;
	switch (err_code) {
	case SQLITE_OK:
		result = Error::OK;
		break;
	case SQLITE_ABORT:
		result = Error::ABORT;
		break;
	case SQLITE_BUSY:
		result = Error::BUSY;
		break;
	case SQLITE_ERROR:
		result = Error::ERROR;
		break;
	case SQLITE_INTERNAL:
		result = Error::INTERNAL;
		break;
	case SQLITE_INTERRUPT:
		result = Error::INTERRUPT;
		break;
	case SQLITE_LOCKED:
		result = Error::LOCKED;
		break;
	case SQLITE_NOMEM:
		result = Error::NOMEM;
		break;
	case SQLITE_PERM:
		result = Error::PERM;
		break;
	case SQLITE_READONLY:
		result = Error::READONLY;
		break;
	default:
		result = Error::UNKNOWN;
		throw std::logic_error("Unknown Error in SQLite!");
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
			std::vector<std::string> row; row.reserve(col_count); 
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

Err_ptr<Result> SQLiteConn::exec_v1(std::string stmt) {
    char *zErrMsg = NULL;
	Result res;
    int rc = sqlite3_exec(this->db_, stmt.c_str(), SQLiteCallBack, &res, &zErrMsg);
    if (rc!=SQLITE_OK) {
		std::cout<<"SQL error {}!"<<sqlite3_errmsg(this->db_)<<std::endl;
        sqlite3_free(zErrMsg);
		return std::unexpected(SQLiteErrorTranform(rc));
    }
    return std::make_unique<Result>(res);
	// FIXME: 如何处理这个 void* ,也许会有更好的做法.
}

SQLiteStmt::SQLiteStmt(sqlite3* db, std::string_view sql_stmt) {
	int rc_ = sqlite3_prepare_v2(db, sql_stmt.data(), sql_stmt.length(), &this->stmt_, NULL);
	this->err_ = SQLiteErrorTranform(rc_);
}

SQLiteStmt::~SQLiteStmt() {
	sqlite3_finalize(this->stmt_);
}

Error SQLiteStmt::fmt(std::vector<std::string_view> params) {
	int bind_param_count = sqlite3_bind_parameter_count(this->stmt_);
	int count = std::min(static_cast<size_t>(bind_param_count), params.size());
	for (int i=0;i<count;i++) {
		sqlite3_bind_text(this->stmt_, i+1, params[i].data(), params[i].length(), SQLITE_TRANSIENT);
	}
	return Error::OK;
} // FIXME: 目前只会返回 Error::OK 

Err<bool> SQLiteStmt::step(void) {
	int rc = sqlite3_step(this->stmt_);
	if (rc==SQLITE_ROW) {
		return true;
	} else if (rc==SQLITE_DONE) {
		return false;
	} else {
		return std::unexpected(SQLiteErrorTranform(rc)); 
	}
} // FIXME: 如果出现问题,似乎没有回旋的余地

Err_ptr<Result> SQLiteStmt::query(void) {
	std::vector<std::string> col_names = this->colNames();
	std::unique_ptr<Result> res = std::make_unique<Result>(col_names);

	while (this->step().value()) {
		res->addRow(this->getRow_TEXT());
	}

	return std::move(res);
}; // FIXME: this->step().value() 处可能会崩溃

unsigned int SQLiteStmt::colCount(void) {
	return sqlite3_column_count(this->stmt_);
}

std::vector<std::string> SQLiteStmt::colNames(void) {
	unsigned int col_count = this->colCount();
	std::vector<std::string> res; res.reserve(col_count);
	for (int i=0;i<col_count;i++) {
		res.emplace_back(sqlite3_column_name(this->stmt_, i));
	}
	return res;
}

Err<ret_str> SQLiteStmt::get_TEXT() {return std::unexpected(Error::OK);} // TODO

std::vector<std::string> SQLiteStmt::getRow_TEXT(void) {
	unsigned int col_count = this->colCount();
	std::vector<std::string> res; res.reserve(col_count);
	for (int i=0;i<col_count;i++) {
		std::string tmp = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt_, i));
		res.push_back(tmp);
		// FIXME: 感觉不太可靠...
	}
	return res;
} // FIXME: 可能会出现在关闭数据库上操作的情况

}