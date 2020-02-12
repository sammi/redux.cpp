#include "pch.h"

namespace redux {

	TEST(Any, DefultConstructor) {
		EXPECT_FALSE(Any());
	}

	TEST(Any, CopySameTypeConstructor) {
		auto a = Any();
		auto b = Any(a);
		EXPECT_FALSE(b);
		b = Any(1);
		EXPECT_TRUE(b);
		EXPECT_EQ(1, b.as<int>());
	}

	TEST(Any, CopyWithConvertConstructor) {
		auto a = Any(1);
		EXPECT_TRUE(a);
		EXPECT_EQ(1, a.as<int>());
	}

	TEST(Any, MoveConstructor) {
		auto a = Any(1);
		auto b = Any(std::move(a));
		EXPECT_EQ(1, b.as<int>());
		EXPECT_TRUE(b);
		EXPECT_FALSE(a);
	}

	TEST(Any, MoveAssignment) {
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
