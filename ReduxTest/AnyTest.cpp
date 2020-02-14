#include "pch.h"

namespace redux {

	TEST(Any, DefultConstructor) {
		EXPECT_FALSE(Any());
	}

	TEST(Any, CopyWithConvertConstructor) {
		auto a = Any(1);
		EXPECT_TRUE(a);
		EXPECT_EQ(1, a.as<int>());
	}

	TEST(Any, CopySameTypeConstructor) {
		auto a = Any(1);
		auto b = Any(a);
		EXPECT_TRUE(a);
		EXPECT_EQ(1, a.as<int>());
		EXPECT_TRUE(b);
		EXPECT_EQ(1, b.as<int>());
	}

	TEST(Any, MoveConstructor) {
		Any a = Any(1);
		auto b = Any(std::move(a));
		EXPECT_EQ(1, b.as<int>());
		EXPECT_TRUE(b);
		EXPECT_FALSE(a);
	}

	TEST(Any, ForwardWithConversionAssignment) {
		Any a = 1;
		EXPECT_EQ(1, a.as<int>());
		Any&& b = 2;
		Any c = std::move(b);
		EXPECT_FALSE(b);
		EXPECT_EQ(2, c.as<int>());
	}
	TEST(Any, MoveSameTypeAssignment) {
		Any a{ 1 };
		Any b{ 2 };
		b = std::move(a);
		EXPECT_FALSE(a);
		EXPECT_EQ(1, b.as<int>());
	}

	TEST(Any, CopyAssignment) {
		Any a{ 1 };
		Any b{ 2 };
		b = a;
		EXPECT_TRUE(a);
		EXPECT_EQ(1, a.as<int>());
		EXPECT_EQ(1, b.as<int>());
	}
}
