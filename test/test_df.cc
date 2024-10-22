#include "../src/dataframe.hpp"
#include <gtest/gtest.h>

TEST(DFtest, df1) {
    DataFrame<int> df1{};
    EXPECT_TRUE(df1.setColN({"abc", "def", "ghi"}));
    EXPECT_TRUE(df1.setColN({"ac", "def", "ghi"}));
    EXPECT_FALSE(df1.setColN({"ac", "ac", "ac"}));
    EXPECT_FALSE(df1.setColN({"abc", "def", "ghi", "kln"}));
    EXPECT_FALSE(df1.setColN({"abc", "kln"}));
    EXPECT_FALSE(df1.hasValue());
    EXPECT_EQ(df1.shape(), std::make_pair(0, 3));

    EXPECT_TRUE(df1.addRow({1, 2, 3}));
    EXPECT_FALSE(df1.addRow({1, 3}));
    for (int i=0;i<50;i++)
        EXPECT_TRUE(df1.addRow({i+1, i+2, i+3}));
    EXPECT_EQ(df1.shape(), std::make_pair(51, 3));

    EXPECT_EQ(df1.access_item(0,2), 3);
    EXPECT_EQ(df1.access_item(26,1), 27);
    EXPECT_EQ(df1.access_item(27,0), 27);
    EXPECT_EQ(df1.access_item(0,"ghi"), 3);
    EXPECT_EQ(df1.access_item(26,"def"), 27);
    EXPECT_EQ(df1.access_item(27,"ac"), 27);

    EXPECT_EQ(df1(0,2), 3);
    EXPECT_EQ(df1(26,1), 27);
    EXPECT_EQ(df1(27,0), 27);
    EXPECT_EQ(df1(0,"ghi"), 3);
    EXPECT_EQ(df1(26,"def"), 27);
    EXPECT_EQ(df1(27,"ac"), 27);
}

TEST(DFtest, df2) {
    DataFrame<std::string> df2{};
    EXPECT_TRUE(df2.setColN({"abc", "def", "ghi", "g++", "gcc", "clang", "msvc"}));
    EXPECT_TRUE(df2.setColN({"ac", "def", "ghi", "gcc", "g++", "clang", "msvc"}));
    EXPECT_FALSE(df2.setColN({"ac", "def", "ghi", "g++", "g++", "clang", "msvc"}));
    EXPECT_FALSE(df2.setColN({"abc", "def", "ghi", "kln"}));
    EXPECT_FALSE(df2.setColN({"abc", "kln"}));
    EXPECT_FALSE(df2.hasValue());
    EXPECT_EQ(df2.shape(), std::make_pair(0, 7));

    EXPECT_TRUE(df2.addRow({"Hello", "World", "Exec", "Hello", "World", "Exec", "Exec"}));
    EXPECT_FALSE(df2.addRow({"What", "up"}));
    for (int i=0;i<50;i++)
        EXPECT_TRUE(df2.addRow({"1", "2", "3", "4", "5", "6", "7"}));
    EXPECT_EQ(df2.shape(), std::make_pair(51, 7));

    EXPECT_EQ(df2.access_item(50,6), "7");
    EXPECT_EQ(df2.access_item(23,1), "2");
    EXPECT_EQ(df2.access_item(22,0), "1");
    EXPECT_EQ(df2.access_item(50,"msvc"), "7");
    EXPECT_EQ(df2.access_item(23,"def"), "2");
    EXPECT_EQ(df2.access_item(22,"ac"), "1");

    EXPECT_EQ(df2(50,6), "7");
    EXPECT_EQ(df2(23,1), "2");
    EXPECT_EQ(df2(22,0), "1");
    EXPECT_EQ(df2(50,"msvc"), "7");
    EXPECT_EQ(df2(23,"def"), "2");
    EXPECT_EQ(df2(22,"ac"), "1");
}