#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <iterator>
#include <utility>
#include <optional>
#include "spdlog/spdlog.h"

#include <sqlite3.h>
#include "sql.h"

SQLResult::SQLResult() {this->line_count_=-1;}
SQLResult::~SQLResult() {}

std::optional<std::pair<std::size_t, std::size_t>> SQLResult::shape(void) const {
	std::size_t row = this->result_.size();
	if (row==0) {
		return std::nullopt;
	}
	std::size_t col = this->result_[0].size();
	if (col==0) {
		return std::nullopt;
	}
	return std::make_pair(row, col);
}

int SQLResult::lineCount(void) const {
	return this->line_count_;
}

ret_str SQLResult::operator()(std::size_t row_num, std::string_view col_name) const {
	auto it = std::find(this->col_.begin(), this->col_.end(), col_name);
	if (it!=this->col_.end()) {
		std::size_t index = std::distance(this->col_.begin(), it);
		return std::string_view(result_[row_num][index]);
	} else {
		spdlog::error("Key {} not found!", col_name);
		return std::nullopt;
	}
}

ret_line SQLResult::operator()(std::size_t row_num) const {
	return std::nullopt;
}

ret_line SQLResult::operator()(std::string_view col_name) const {
	return std::nullopt;
}

SQLResult SQLHandle::create(void) {return SQLResult();};
SQLResult SQLHandle::insert(void) {return SQLResult();};
SQLResult SQLHandle::update(void) {return SQLResult();};
SQLResult SQLHandle::del(void) {return SQLResult();};

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

SQLResult SQLiteHandle::exec(std::string stmt) {
	SQLiteResult sqlite_res;
	sqlite3_stmt* sql_stmt;

	int rc = sqlite3_prepare_v2(this->db_, stmt.c_str(), stmt.length(), &sql_stmt, NULL);
	if (rc==SQLITE_OK) {
		while (sqlite3_step(sql_stmt)==SQLITE_ROW) {
			int col_count = sqlite3_column_count(sql_stmt);
			if (sqlite_res.line_count_==-1) {
				for (int i=0;i<col_count;i++) {
					sqlite_res.col_.emplace_back(sqlite3_column_name(sql_stmt, i));
				}
				sqlite_res.line_count_ = 1;
			} else {
				sqlite_res.line_count_++;
			}

			std::vector<std::string> row;
			for (int i=0;i<col_count;i++) {
				const unsigned char* text_value = sqlite3_column_text(sql_stmt, i);
				if (text_value) {
                	row.push_back(reinterpret_cast<const char*>(text_value));  
            	} else {
                	row.push_back("");  
            	}
			}
			sqlite_res.result_.push_back(row);
		}
	} else {
		spdlog::error("SQL error {}!", sqlite3_errmsg(this->db_));
	}

	sqlite3_finalize(sql_stmt);
	return sqlite_res;
}