#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <iterator>
#include <utility>
#include <optional>
#include <sstream>
#include "spdlog/spdlog.h"

#include <sqlite3.h>
#include "sql.h"

std::string SQLHandle::getTypeName(ColType colt) {
	std::string res;
	switch (colt) {
	case SQLHandle::ColType::TEXT :
		res = "TEXT";
		break;
	case SQLHandle::ColType::BLOB :
		res = "BLOB";
		break;
	case SQLHandle::ColType::REAL :
		res = "REAL";
		break;
	case SQLHandle::ColType::INTEGER :
		res = "INTEGER";
		break;
	default:
		break;
	} return res;
}

bool SQLHandle::create(
		std::string table_name, 
		std::vector<std::string> col_names, 
		std::vector<SQLHandle::ColType> col_types,
		bool create_if_not_exists
	) {
	
	std::ostringstream stmt;
	if (col_names.size()!=col_types.size() || col_names.size()==0)
		return false;
	int col_count = col_names.size();
	stmt << "CREATE TABLE "
		<<(create_if_not_exists ? "IF NOT EXISTS ":" ")
		<<table_name<< " (";
	for (int i=0;i<col_count;i++) {
		stmt <<" "<<col_names[i]<<" "
			<<SQLHandle::getTypeName(col_types[i])
			<<((col_count-i)>1 ? ",":"");
			// 保证最后一项不输出逗号
	} stmt << ");";
	std::cout<<stmt.str()<<std::endl;

	return this->exec(stmt.str()).isSuccess();
}

SQLResult SQLHandle::select(void) {return SQLResult(SQLITE_OK);}
bool SQLHandle::insert(void) {return true;}
bool SQLHandle::update(void) {return true;}
bool SQLHandle::del(void) {return true;}

SQLResult SQLHandle::getTables(void) {
	constexpr char stmt[] = "SELECT * FROM sqlite_master WHERE type='table';";
	return this->exec(std::string(stmt));
}

SQLiteHandle::SQLiteHandle(std::string_view db_name) {
    int64_t rc = sqlite3_open(db_name.data(), &(this->db_));
    if (rc) {
		spdlog::error("Can't open database {}!", sqlite3_errmsg(this->db_));
        sqlite3_close(this->db_);
        exit(1);
    }
}

SQLiteHandle::~SQLiteHandle() {
    sqlite3_close(this->db_);
}

// FIXME:所有的列全视为 TEXT,需要改进
SQLResult SQLiteHandle::exec(std::string stmt) {
	sqlite3_stmt* sql_stmt;
	int rc = sqlite3_prepare_v2(this->db_, stmt.c_str(), stmt.length(), &sql_stmt, NULL);
	SQLResult sqlite_res{rc};
	if (rc==SQLITE_OK) {
		while (sqlite3_step(sql_stmt)==SQLITE_ROW) {
			int col_count = sqlite3_column_count(sql_stmt);
			if (!sqlite_res.hasValue()) {
				std::vector<std::string> col_names;
				for (int i=0;i<col_count;i++) {
					col_names.emplace_back(sqlite3_column_name(sql_stmt, i));
				}
				sqlite_res.setColN(col_names);
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
			sqlite_res.addRow(row);
		}
	} else {
		spdlog::error("SQL error {}!", sqlite3_errmsg(this->db_));
	}

	sqlite3_finalize(sql_stmt);
	return sqlite_res;
}