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

using SQL::Err;
using SQL::Err_ptr;

SQL::Error SQL::SQLiteErrorTranform(int err_code) {
	SQL::Error result;
	switch (err_code) {
	case SQLITE_OK:
		result = SQL::Error::OK;
		break;
	default:
		result = SQL::Error::UNKNOWN;
		break;
	}
	return result;
}

std::string SQL::Conn::getTypeName(ColType colt) {
	std::string res;
	switch (colt) {
	case SQL::Conn::ColType::TEXT :
		res = "TEXT";
		break;
	case SQL::Conn::ColType::BLOB :
		res = "BLOB";
		break;
	case SQL::Conn::ColType::REAL :
		res = "REAL";
		break;
	case SQL::Conn::ColType::INTEGER :
		res = "INTEGER";
		break;
	default:
		break;
	} return res;
}

SQL::Error SQL::Conn::create(
		std::string table_name, 
		std::vector<std::string> col_names, 
		std::vector<SQL::Conn::ColType> col_types,
		bool create_if_not_exists
	) {
	
	std::ostringstream stmt;
	if (col_names.size()!=col_types.size() || col_names.size()==0)
		return SQL::Error::PARAM_ERROR;
	int col_count = col_names.size();
	stmt << "CREATE TABLE "
		<<(create_if_not_exists ? "IF NOT EXISTS ":" ")
		<<table_name<< " (";
	for (int i=0;i<col_count;i++) {
		stmt <<" "<<col_names[i]<<" "
			<<SQL::Conn::getTypeName(col_types[i])
			<<((col_count-i)>1 ? ",":"");
			// 保证最后一项不输出逗号
	} stmt << ");";
	std::cout<<stmt.str()<<std::endl;

	Err_ptr<Result> epr = this->exec(stmt.str());
	if (epr.has_value()) {
		return Error::OK;
	} else {
		return epr.error();
	}
}

Err_ptr<SQL::Result> SQL::Conn::selectAll(std::string table_name) {
	constexpr char stmt[] = "SELECT * FROM ?";
	return std::make_unique<SQL::Result>();
} // TODO

Err_ptr<SQL::Result> SQL::Conn::select(
	std::string table_name, 
	std::vector<std::string> col_names,
	std::string sub_stmt
	) {

	constexpr char stmt[] = "SELECT  FROM ";
	return std::make_unique<SQL::Result>();
} // TODO

Err_ptr<SQL::Result> SQL::Conn::select(
	std::string table_name,
	std::vector<std::string> col_names
	) {
	return std::make_unique<SQL::Result>();
} // TODO

bool SQL::Conn::insert(void) {return true;} // TODO
bool SQL::Conn::update(void) {return true;} // TODO
bool SQL::Conn::del(void) {return true;} // TODO

Err_ptr<SQL::Result> SQL::Conn::getTables(void) {
	constexpr char stmt[] = "SELECT * FROM sqlite_master WHERE type='table';";
	return this->exec(std::string(stmt));
}

std::unique_ptr<SQL::Stmt> SQL::SQLiteConn::preCompile(std::string_view stmt) {
	std::unique_ptr<SQL::SQLiteStmt> sqlite_stmt = std::make_unique<SQL::SQLiteStmt>(this->db_, stmt);
	return std::move(sqlite_stmt);
}

SQL::SQLiteConn::SQLiteConn(std::string_view db_path) {
    int64_t rc = sqlite3_open(db_path.data(), &(this->db_));
    if (rc) {
		std::cout<<"Can't open database {}!"<<sqlite3_errmsg(this->db_)<<std::endl;
        sqlite3_close(this->db_);
        exit(1);
    }
}

SQL::SQLiteConn::~SQLiteConn() {
    sqlite3_close(this->db_);
}

// FIXME:所有的列全视为 TEXT,需要改进
Err_ptr<SQL::Result> SQL::SQLiteConn::exec(std::string stmt) {
	Err_ptr<SQL::Result> res;

	sqlite3_stmt* sql_stmt;
	int rc = sqlite3_prepare_v2(this->db_, stmt.c_str(), stmt.length(), &sql_stmt, NULL);
	if (rc==SQLITE_OK) {
		std::unique_ptr<SQL::Result> sqlite_res = std::make_unique<SQL::Result>();
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
		res = std::unexpected(SQL::SQLiteErrorTranform(rc));
	}

	sqlite3_finalize(sql_stmt);
	return res;
}

SQL::SQLiteStmt::SQLiteStmt(sqlite3* db, std::string_view sql_stmt) : db_(db) {
	int rc_ = sqlite3_prepare_v2(db_, sql_stmt.data(), sql_stmt.length(), &this->stmt_, NULL);
	this->err_ = SQL::SQLiteErrorTranform(rc_);
}

SQL::SQLiteStmt::~SQLiteStmt() {}

SQL::Error SQL::SQLiteStmt::fmt(std::vector<std::string_view> params) {
	int bind_param_count = sqlite3_bind_parameter_count(this->stmt_);
	int count = std::min(static_cast<size_t>(bind_param_count), params.size());
	for (int i=0;i<count;i++) {
		sqlite3_bind_text(this->stmt_, i+1, params[i].data(), params[i].length(), SQLITE_TRANSIENT);
	}
	return SQL::Error::OK;
} // FIXME: 目前只会返回 Error::OK 

SQL::Error SQL::SQLiteStmt::step(void) {return Error::OK;} // TODO

Err_ptr<SQL::Result> SQL::SQLiteStmt::query() {return std::make_unique<SQL::Result>();};

Err<int> SQL::SQLiteStmt::colCount(void) {return std::unexpected(Error::OK);} // TODO
Err<SQL::ret_str> SQL::SQLiteStmt::colName(void) {return std::unexpected(Error::OK);} // TODO
Err<SQL::ret_line> SQL::SQLiteStmt::colNames(void) {return std::unexpected(Error::OK);} // TODO

Err<SQL::ret_str> SQL::SQLiteStmt::get_TEXT() {return std::unexpected(Error::OK);} // TODO
Err<SQL::ret_line> SQL::SQLiteStmt::getRow_TEXT() {return std::unexpected(Error::OK);} // TODO