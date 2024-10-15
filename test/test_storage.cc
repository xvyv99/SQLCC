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
		{"Else if", "CI\\DI"},
	};
	Meta m1(mm1);
	EXPECT_EQ(Meta::fromString("{o:p,q:r}"), mm1);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r,}"), mm1);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,}q:r"), std::nullopt);	
	EXPECT_EQ(Meta::fromString("{o:p,q:r,"), std::nullopt);	

	Meta m1_copy(Meta::fromString(m1.toString()).value());
	EXPECT_EQ(m1_copy, m1);	

	Meta m2(mm2);
	
	Meta m2_copy(Meta::fromString(m2.toString()).value());
	EXPECT_EQ(m2_copy, m2);	
}
