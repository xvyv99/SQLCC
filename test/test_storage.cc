#include "../src/storage.h"
#include <gtest/gtest.h>

TEST(StorageTest, Meta) {
	meta_map mm1 = {
		{"o", "p"},
		{"q", "r"},
	}; 
	Meta m1(mm1);
	//EXPECT_EQ(m1.toString(), std::string("{o:p,q:r}"));	
	EXPECT_EQ(m1.fromString("{o:p,q:r}"), mm1);	
	EXPECT_EQ(m1.fromString("{o:p,q:r,}"), mm1);	
	EXPECT_EQ(m1.fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(m1.fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(m1.fromString("{o:p,}q:r"), std::nullopt);	
	EXPECT_EQ(m1.fromString("{o:p,q:r,"), std::nullopt);	
}
