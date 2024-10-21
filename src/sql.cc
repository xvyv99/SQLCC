#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <iterator>
#include <utility>
#include <optional>
#include <spdlog/spdlog.h>

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
	auto it = std::find(this->row_.begin(), this->row_.end(), col_name);
	if (it!=this->row_.end()) {
		std::size_t index = std::distance(this->row_.begin(), it);
		return std::string_view(result_[row_num][index]);
	} else {
		spdlog::error("Key {} not found!", col_name);
		return std::nullopt;
	}
}

ret_line SQLResult::operator()(std::size_t row_num) const {
	
}

ret_line SQLResult::operator()(std::string_view col_name) const {
		
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

SQLResult SQLiteHandle::exec(std::string stmt) {
    char *zErrMsg = NULL;
	SQLiteResult sqlite_res;
    int64_t rc = rc = sqlite3_exec(this->db_, stmt.c_str(), CallBack, &sqlite_res, &zErrMsg);
    if (rc!=SQLITE_OK) {
		spdlog::error("SQL error: {}",zErrMsg);
        sqlite3_free(zErrMsg);
    }
	spdlog::info("Result shape:{}", sqlite_res.shape());
    return sqlite_res;
}

std::optional<SQLResult> SQLiteHandle::query(std::string_view stmt) {
	SQLiteResult sqlite_res;
	sqlite3_stmt* sql_stmt;
	// const char* tail;

	int64_t rc = sqlite3_prepare_v2(this->db_, stmt.data(), stmt.length(), &sql_stmt, NULL);
	if (rc==SQLITE_OK) {
		while (sqlite3_step(sql_stmt)==SQLITE_ROW) {
			
		}
	} else {
		spdlog::error("SQL error {}!", sqlite3_errmsg(this->db_));
		return std::nullopt;
	}
	
	sqlite3_finalize(sql_stmt);
}

int SQLiteCallBack(SQLiteResult &sqlite_res, int argc, char **argv, char **azColName) {
	std::vector<std::string> row;

	if (sqlite_res.line_count_==-1) {
		for (int i=0; i<argc; i++) {
			sqlite_res.row_.emplace_back(std::string(azColName[i]));
			spdlog::debug("{}", azColName[i]);
		}
		sqlite_res.line_count_ = 1;
	} else {
		sqlite_res.line_count_++;
	}

    for (int i = 0; i < argc; i++) {
		row.emplace_back(argv[i]);
		spdlog::info("{} = {}\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
	sqlite_res.result_.push_back(row);

    return 0;
}

/**
 * @brief 回调函数,用于打印结果SQL查询结果,来源于 [SQLite In 5 Minutes Or Less](https://sqlite.org/quickstart.html).
 */
int CallBack(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int64_t i = 0; i < argc; i++) {
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
