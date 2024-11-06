#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <iterator>
#include <utility>
#include <optional>
#include <expected>
#include <exception>
#include <sstream>
#include <memory>

#include <sqlite3.h>
#include "sql.h"
namespace SQL {

std::string Conn::getTypeName(ColType colt) {
	std::string res;
	switch (colt) {
	case Conn::ColType::TEXT :
		res = "TEXT";
		break;
	case Conn::ColType::BLOB :
		res = "BLOB";
		break;
	case Conn::ColType::REAL :
		res = "REAL";
		break;
	case Conn::ColType::INTEGER :
		res = "INTEGER";
		break;
	default:
		break;
	} return res;
}

Error Conn::create(
		std::string table_name, 
		std::vector<std::string> col_names, 
		std::vector<Conn::ColType> col_types,
		bool create_if_not_exists
	) {
	
	std::ostringstream stmt;
	if (col_names.size()!=col_types.size() || col_names.size()==0)
		return Error::PARAM_ERROR;
	int col_count = col_names.size();
	stmt << "CREATE TABLE "
		<<(create_if_not_exists ? "IF NOT EXISTS ":" ")
		<<table_name<< " (";
	for (int i=0;i<col_count;i++) {
		stmt <<" "<<col_names[i]<<" "
			<<Conn::getTypeName(col_types[i])
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

Err_ptr<Result> Conn::selectAll(std::string table_name) {
	constexpr char stmt[] = "SELECT * FROM ?";
	return std::make_unique<Result>();
} // TODO

Err_ptr<Result> Conn::select(
	std::string table_name, 
	std::vector<std::string> col_names,
	std::string sub_stmt
	) {

	constexpr char stmt[] = "SELECT  FROM ";
	return std::make_unique<Result>();
} // TODO

Err_ptr<Result> Conn::select(
	std::string table_name,
	std::vector<std::string> col_names
	) {
	return std::make_unique<Result>();
} // TODO

bool Conn::insert(void) {return true;} // TODO
bool Conn::update(void) {return true;} // TODO
bool Conn::del(void) {return true;} // TODO

Err_ptr<Result> Conn::getTables(void) {
	constexpr char stmt[] = "SELECT * FROM sqlite_master WHERE type='table';";
	return this->exec(std::string(stmt));
}

}