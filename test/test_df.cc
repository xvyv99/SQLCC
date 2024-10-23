#include "../src/dataframe.hpp"
#include <gtest/gtest.h>

TEST(DFtest, int) {
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

    EXPECT_EQ(df1.accessItem(0,2), 3);
    EXPECT_EQ(df1.accessItem(26,1), 27);
    EXPECT_EQ(df1.accessItem(27,0), 27);
    EXPECT_EQ(df1.accessItem(0,"ghi"), 3);
    EXPECT_EQ(df1.accessItem(26,"def"), 27);
    EXPECT_EQ(df1.accessItem(27,"ac"), 27);

    EXPECT_EQ(df1(0,2), 3);
    EXPECT_EQ(df1(26,1), 27);
    EXPECT_EQ(df1(27,0), 27);
    EXPECT_EQ(df1(0,"ghi"), 3);
    EXPECT_EQ(df1(26,"def"), 27);
    EXPECT_EQ(df1(27,"ac"), 27);

    std::vector<int> r1 = {2, 2, 2, 2, 2};
    std::vector<int> r2 = {4, 4, 4, 4, 4};
    std::vector<int> r3 = {1, 2, 3, 0, 4};
    DataFrame<int> df3{};
    EXPECT_TRUE(df3.setColN({"abcdf", "", "ghiasd", "klczvxn", "  "}));
    for (int i=0;i<5;i++)
        EXPECT_TRUE(df3.addRow({1, 2, 3, 0, 4}));
    EXPECT_EQ(df3.accessCol("").value(), r1);
    EXPECT_EQ(df3.accessCol("  ").value(), r2);
    EXPECT_EQ(df3.accessRow(1).value(), r3);
    EXPECT_EQ(df3.accessRow(3).value(), r3);
    EXPECT_EQ(df3[""].value(), r1);
    EXPECT_EQ(df3["  "].value(), r2);
    EXPECT_EQ(df3[1].value(), r3);
    EXPECT_EQ(df3[4].value(), r3);
}

TEST(DFtest, string) {
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

    EXPECT_EQ(df2.accessItem(50,6), "7");
    EXPECT_EQ(df2.accessItem(23,1), "2");
    EXPECT_EQ(df2.accessItem(22,0), "1");
    EXPECT_EQ(df2.accessItem(50,"msvc"), "7");
    EXPECT_EQ(df2.accessItem(23,"def"), "2");
    EXPECT_EQ(df2.accessItem(22,"ac"), "1");

    EXPECT_EQ(df2(50,6), "7");
    EXPECT_EQ(df2(23,1), "2");
    EXPECT_EQ(df2(22,0), "1");
    EXPECT_EQ(df2(50,"msvc"), "7");
    EXPECT_EQ(df2(23,"def"), "2");
    EXPECT_EQ(df2(22,"ac"), "1");

    std::vector<std::string> r4 = {"2", "2", "2", "2", "2"};
    std::vector<std::string> r5 = {"4", "4", "4", "4", "4"};
    std::vector<std::string> r6 = {"1", "2", "3", "4"};
    DataFrame<std::string> df4{};
    EXPECT_TRUE(df4.setColN({"abc", "def", "ghi", "kln"}));
    for (int i=0;i<5;i++)
        EXPECT_TRUE(df4.addRow({"1", "2", "3", "4"}));
    EXPECT_EQ(df4.accessCol("def").value(), r4);
    EXPECT_EQ(df4.accessCol("kln").value(), r5);
    EXPECT_EQ(df4.accessRow(1).value(), r6);
    EXPECT_EQ(df4.accessRow(2).value(), r6);
    EXPECT_EQ(df4["def"].value(), r4);
    EXPECT_EQ(df4["kln"].value(), r5);
    EXPECT_EQ(df4[1].value(), r6);
    EXPECT_EQ(df4[2].value(), r6);
}