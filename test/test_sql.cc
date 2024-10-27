#include "../src/sql.h"
#include <gtest/gtest.h>
#include <optional>
#include <expected>
#include <memory>
#include <iostream>
#include <set>

using namespace SQL;

namespace {

class SQLiteTest : public testing::Test {
protected:
	void SetUp() override {
	//	SQLiteConn sqlh = SQLiteConn("./test.db");
	//	sqlh.exec("CREATE TABLE BASE {NAME TEXT, PATH TEXT};");
	}

	void TearDown() override {

	}
	
	//SQLiteConn sqlh;
};	

TEST_F(SQLiteTest, exec) {
	SQLiteConn sqlh = SQLiteConn("./test.db");
	sqlh.exec("CREATE TABLE IF NOT EXISTS BASE (NAME TEXT, PATH TEXT);");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST1\", \"TEST2\");");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST3\", \"TEST4\");");
	std::unique_ptr<Result> sqlr1 = sqlh.exec("SELECT NAME,PATH FROM BASE;").value();
	//ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1->accessItem(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1->accessItem(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1->accessItem(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1->accessItem(1, "PATH"), "TEST4");
}

TEST_F(SQLiteTest, create) {
	SQLiteConn sqlh = SQLiteConn("./test.db");
	sqlh.create(
		"Hello", 
		{"ACD", "asd", "enti"}, 
		{Conn::ColType::TEXT, Conn::ColType::TEXT, Conn::ColType::TEXT},
		true
	);
	sqlh.create(
		"Gello", 
		{"ACD", "asd", "enti"}, 
		{Conn::ColType::TEXT, Conn::ColType::TEXT, Conn::ColType::TEXT},
		true
	);
	sqlh.create(
		"World", 
		{"ACD", "asd", "enti"}, 
		{Conn::ColType::INTEGER, Conn::ColType::BLOB, Conn::ColType::TEXT},
		true
	);
	std::unique_ptr<Result> sr = sqlh.getTables().value();
	//ASSERT_TRUE(sr.isSuccess());
	auto srv = sr->accessCol("name").value();
	std::set<std::string> ss = {"BASE", "Hello", "Gello", "World"};
	std::set<std::string> so{srv.begin(), srv.end()};
	EXPECT_EQ(so, ss);
}

}
