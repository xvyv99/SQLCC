#include "../src/dataframe.h"
#include <gtest/gtest.h>

TEST(DFtest, df1) {
    DataFrame<int> df1{};
    EXPECT_TRUE(df1.setColN({"abc", "def", "ghi"}));
    EXPECT_TRUE(df1.setColN({"ac", "def", "ghi"}));
    EXPECT_FALSE(df1.setColN({"abc", "def", "ghi", "kln"}));
    EXPECT_FALSE(df1.setColN({"abc", "kln"}));
    EXPECT_FALSE(df1.hasValue());
    EXPECT_EQ(df1.shape(), std::make_pair(0, 3));

    EXPECT_TRUE(df1.addRow({1, 2, 3}));
    EXPECT_FALSE(df1.addRow({1, 3}));
    for (int i=0;i<50;i++)
        EXPECT_TRUE(df1.addRow({i+1, i+2, i+3}));
    EXPECT_EQ(df1.shape(), std::make_pair(51, 3));
}
