#include "../unittest.h"

#include "hecate.h"

#include <vector>

namespace {
	int foo(int a, int b) {
		return a + b;
	}

	struct Bar {
		int m_Value = 5;

		int add_last(int x) {
			int result = x + m_Value;
			m_Value = x;
			return result;
		}
	};
}

namespace test {
	TEST_CASE("function_global", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn = foo;

		int x = fn(2, 3);

		REQUIRE(x == 5);
	}


	TEST_CASE("function_lambda_stateless", "[hecate::util]") {
		using namespace hecate::util;

		// NOTE - stateless lambda may decay to a function pointer
		Function<int(int, int)> fn = [](int a, int b) {
			return a + b;
		};

		int x = fn(2, 3);

		REQUIRE(x == 5);
	}

	TEST_CASE("function_lambda_stateful", "[hecate::util]") {
		using namespace hecate::util;

		Bar b;

		Function<int(int)> fn_a = [&](int a) {
			return b.add_last(a);
		};

		int x = fn_a(4);

		REQUIRE(x == 9);
		REQUIRE(b.m_Value == 4);

		int y = fn_a(10);

		REQUIRE(y == 14);
		REQUIRE(b.m_Value == 10);


		Function<int(int)> fn_b = fn_a;
		int z = fn_b(1);

		REQUIRE(z == 11);
	}

	TEST_CASE("function_member", "[hecate::util]") {
		using namespace hecate::util;

		Bar b;

		Function<int(Bar&, int)> fn = &Bar::add_last;

		int x = fn(b, 4);

		REQUIRE(x == 9);
		REQUIRE(b.m_Value == 4);
	}
}