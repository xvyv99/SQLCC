#include "../src/storage.h"
#include <gtest/gtest.h>

TEST(StorageTest, Meta) {
	meta_map mm1 = {
		{"o", "p"},
		{"q", "r"},
	}; 
	meta_map mm2 = {
		{"AB", "CD"},
		{"EF", "quware"},
		{"Quant Inf", "I/O Map"},
		{"Else if", "CI\DI"},
	};
	Meta m1(mm1);
	EXPECT_EQ(Meta::fromString("{o:p,q:r}"), mm1);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r,}"), mm1);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,}q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r,"), std::nullopt);	

	Meta m1_copy(Meta::fromString(m1.toString()));
	EXPECT_EQ(m1.toString(), std::string("{o:p,q:r}"));	

	Meta m2(mm2);
	
}
