#include "../src/sql.h"
#include <gtest/gtest.h>
#include <optional>
#include <iostream>

namespace {

class SQLiteTest : public testing::Test {
protected:
	void SetUp() override {
	//	SQLiteHandle sqlh = SQLiteHandle("./test.db");
	//	sqlh.exec("CREATE TABLE BASE {NAME TEXT, PATH TEXT};");
	}

	void TearDown() override {

	}
	
	//SQLiteHandle sqlh;
};	

TEST_F(SQLiteTest, exec) {
	SQLiteHandle sqlh = SQLiteHandle("./test.db");
	sqlh.exec("CREATE TABLE IF NOT EXISTS BASE (NAME TEXT, PATH TEXT);");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST1\", \"TEST2\");");
	sqlh.exec("INSERT INTO BASE VALUES (\"TEST3\", \"TEST4\");");
	SQLResult sqlr1 = sqlh.exec("SELECT NAME,PATH FROM BASE;");
	ASSERT_TRUE(sqlr1.hasValue());
	EXPECT_EQ(sqlr1(0, "NAME"), "TEST1");
	EXPECT_EQ(sqlr1(0, "PATH"), "TEST2");
	EXPECT_EQ(sqlr1(1, "NAME"), "TEST3");
	EXPECT_EQ(sqlr1(1, "PATH"), "TEST4");
}

TEST_F(SQLiteTest, create) {
	SQLiteHandle sqlh = SQLiteHandle("./test.db");
	EXPECT_TRUE(sqlh.create(
		"Hello", 
		{"ACD", "asd", "enti"}, 
		{SQLHandle::ColType::TEXT, SQLHandle::ColType::TEXT, SQLHandle::ColType::TEXT},
		true
	));
		EXPECT_TRUE(sqlh.create(
		"Gello", 
		{"ACD", "asd", "enti"}, 
		{SQLHandle::ColType::TEXT, SQLHandle::ColType::TEXT, SQLHandle::ColType::TEXT},
		true
	));
	EXPECT_TRUE(sqlh.create(
		"World", 
		{"ACD", "asd", "enti"}, 
		{SQLHandle::ColType::INTEGER, SQLHandle::ColType::BLOB, SQLHandle::ColType::TEXT},
		true
	));
	SQLResult sr = sqlh.getTables();
	ASSERT_TRUE(sr.isSuccess());
	std::cout << sr.shape().first <<sr.shape().second;
}

}
