#include "../src/sqlite.h"
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
	sqlh.exec("CREATE TABLE BASE (NAME TEXT, PATH TEXT);");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST1\", \"TEST2\");");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST3\", \"TEST4\");");
	std::unique_ptr<Result> sqlr1 = sqlh.exec("SELECT NAME,PATH FROM BASE;").value();
	//ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1->accessItem(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1->accessItem(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1->accessItem(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1->accessItem(1, "PATH"), "TEST4");
	sqlh.exec("DROP TABLE BASE;");
}

TEST_F(SQLiteTest, exec_v1) {
	SQLiteConn sqlh = SQLiteConn("./test.db");
	sqlh.exec_v1("CREATE TABLE BASE (NAME TEXT, PATH TEXT);");
	sqlh.exec_v1("INSERT INTO BASE VALUES (\"TEST1\", \"TEST2\");");
	sqlh.exec_v1("INSERT INTO BASE VALUES (\"TEST3\", \"TEST4\");");
	std::unique_ptr<Result> sqlr1 = sqlh.exec_v1("SELECT NAME,PATH FROM BASE;").value();
	//ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1->accessItem(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1->accessItem(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1->accessItem(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1->accessItem(1, "PATH"), "TEST4");
	sqlh.exec_v1("DROP TABLE BASE;");
}

TEST_F(SQLiteTest, query) {
	SQLiteConn sqlh = SQLiteConn("./test.db");
	sqlh.preCompile("CREATE TABLE BASE (NAME TEXT, PATH TEXT);")->query();
	sqlh.preCompile("INSERT INTO BASE VALUES (\"TEST1\", \"TEST2\");")->query();
	sqlh.preCompile("INSERT INTO BASE VALUES (\"TEST3\", \"TEST4\");")->query();
	auto s4 = sqlh.preCompile("SELECT NAME,PATH FROM BASE;");

	std::unique_ptr<Result> sqlr1 = s4->query().value();
	//ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1->accessItem(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1->accessItem(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1->accessItem(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1->accessItem(1, "PATH"), "TEST4");
	sqlh.preCompile("DROP TABLE BASE;")->query();
}

TEST_F(SQLiteTest, fmt) {
	SQLiteConn sqlh = SQLiteConn("./test.db");
	sqlh.preCompile("CREATE TABLE BASE (NAME TEXT, PATH TEXT);");
	auto s2 = sqlh.preCompile("INSERT INTO BASE VALUES (?, ?);");
	auto s3 = sqlh.preCompile("INSERT INTO BASE VALUES (?, ?);");
	std::vector<std::string_view> param1 = {"TEST1", "TEST2"};
	std::vector<std::string_view> param2 = {"TEST3", "TEST4"};
	//ASSERT_TRUE(s2->fmt(param1));
	s2->fmt_loose(param1);
	s3->fmt_loose(param2);
	s2->query(); s3->query();
	auto s4 = sqlh.preCompile("SELECT NAME,PATH FROM BASE;");

	std::unique_ptr<Result> sqlr1 = s4->query().value();
	//ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1->accessItem(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1->accessItem(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1->accessItem(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1->accessItem(1, "PATH"), "TEST4");
	sqlh.preCompile("DROP TABLE BASE;")->query();
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
	std::set<std::string> ss = {"Hello", "Gello", "World"};
	std::set<std::string> so{srv.begin(), srv.end()};
	EXPECT_EQ(so, ss);
}

}
