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

	TEST_CASE("function_copy_constructor", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn1 = foo;
		Function<int(int, int)> fn2 = fn1;

		REQUIRE(fn1(3, 7) == 10);
		REQUIRE(fn2(3, 7) == 10);
	}

	TEST_CASE("function_copy_assignment", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn1 = foo;
		Function<int(int, int)> fn2 = [](int a, int b) { return a * b; };

		REQUIRE(fn2(3, 7) == 21);

		fn2 = fn1;

		REQUIRE(fn2(3, 7) == 10);
	}

	TEST_CASE("function_move_constructor", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn1 = foo;
		Function<int(int, int)> fn2 = std::move(fn1);

		REQUIRE(fn2(5, 8) == 13);
	}

	TEST_CASE("function_move_assignment", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn1 = foo;
		Function<int(int, int)> fn2 = [](int a, int b) { return a * b; };

		fn2 = std::move(fn1);

		REQUIRE(fn2(5, 8) == 13);
	}

	TEST_CASE("function_void_return", "[hecate::util]") {
		using namespace hecate::util;

		int result = 0;
		Function<void(int)> fn = [&result](int x) {
			result = x * 2;
		};

		fn(5);
		REQUIRE(result == 10);

		fn(7);
		REQUIRE(result == 14);
	}

	TEST_CASE("function_no_args", "[hecate::util]") {
		using namespace hecate::util;

		int counter = 0;
		Function<int()> fn = [&counter]() {
			return ++counter;
		};

		REQUIRE(fn() == 1);
		REQUIRE(fn() == 2);
		REQUIRE(fn() == 3);
	}

	TEST_CASE("function_multiple_calls", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int, int)> fn = foo;

		REQUIRE(fn(1, 2) == 3);
		REQUIRE(fn(10, 20) == 30);
		REQUIRE(fn(-5, 5) == 0);
		REQUIRE(fn(100, 200) == 300);
	}

	TEST_CASE("function_lambda_capture_by_value", "[hecate::util]") {
		using namespace hecate::util;

		int multiplier = 3;
		Function<int(int)> fn = [multiplier](int x) {
			return x * multiplier;
		};

		REQUIRE(fn(5) == 15);
		REQUIRE(fn(10) == 30);

		// Changing multiplier doesn't affect fn since it was captured by value
		multiplier = 5;
		REQUIRE(fn(5) == 15);
	}

	TEST_CASE("function_lambda_capture_by_reference", "[hecate::util]") {
		using namespace hecate::util;

		int multiplier = 3;
		Function<int(int)> fn = [&multiplier](int x) {
			return x * multiplier;
		};

		REQUIRE(fn(5) == 15);

		// Changing multiplier affects fn since it was captured by reference
		multiplier = 5;
		REQUIRE(fn(5) == 25);
	}

	TEST_CASE("function_return_reference", "[hecate::util]") {
		using namespace hecate::util;

		static int global_value = 42;
		Function<int&()> fn = []() -> int& {
			return global_value;
		};

		int& ref = fn();
		REQUIRE(ref == 42);

		ref = 100;
		REQUIRE(global_value == 100);
	}

	TEST_CASE("function_const_reference_parameter", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(const std::vector<int>&)> fn = [](const std::vector<int>& vec) {
			int sum = 0;
			for (int val : vec) {
				sum += val;
			}
			return sum;
		};

		std::vector<int> data = {1, 2, 3, 4, 5};
		REQUIRE(fn(data) == 15);
	}

	TEST_CASE("function_mutable_lambda", "[hecate::util]") {
		using namespace hecate::util;

		Function<int()> fn = [counter = 0]() mutable {
			return ++counter;
		};

		REQUIRE(fn() == 1);
		REQUIRE(fn() == 2);
		REQUIRE(fn() == 3);
	}

	TEST_CASE("function_nested_calls", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int)> add_one = [](int x) { return x + 1; };
		Function<int(int)> double_it = [](int x) { return x * 2; };

		// Test composition by calling sequentially
		int step1 = add_one(5);
		int result1 = double_it(step1);
		REQUIRE(result1 == 12);  // (5 + 1) * 2 = 12

		int step2 = add_one(10);
		int result2 = double_it(step2);
		REQUIRE(result2 == 22); // (10 + 1) * 2 = 22
	}

	TEST_CASE("function_bool_return", "[hecate::util]") {
		using namespace hecate::util;

		Function<bool(int)> is_even = [](int x) {
			return x % 2 == 0;
		};

		REQUIRE(is_even(4) == true);
		REQUIRE(is_even(7) == false);
		REQUIRE(is_even(0) == true);
		REQUIRE(is_even(-2) == true);
	}

	TEST_CASE("function_string_operations", "[hecate::util]") {
		using namespace hecate::util;

		Function<std::string(const std::string&, const std::string&)> concat =
			[](const std::string& a, const std::string& b) {
				return a + b;
			};

		REQUIRE(concat("Hello", "World") == "HelloWorld");
		REQUIRE(concat("foo", "bar") == "foobar");
	}

	TEST_CASE("function_reassignment", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int)> fn = [](int x) { return x + 1; };
		REQUIRE(fn(5) == 6);

		fn = [](int x) { return x * 2; };
		REQUIRE(fn(5) == 10);

		fn = [](int x) { return x - 3; };
		REQUIRE(fn(5) == 2);
	}

	TEST_CASE("function_empty_state", "[hecate::util]") {
		using namespace hecate::util;

		// Default constructed function is empty
		Function<int(int)> fn1;
		REQUIRE(fn1.empty());
		REQUIRE(!fn1);

		// Assigned function is not empty
		fn1 = [](int x) { return x + 1; };
		REQUIRE(!fn1.empty());
		REQUIRE(fn1);
		REQUIRE(fn1(5) == 6);
	}

	TEST_CASE("function_empty_call_throws", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int)> fn;
		REQUIRE_THROWS_AS(fn(5), bad_function_call);
	}

	TEST_CASE("function_perfect_forwarding", "[hecate::util]") {
		using namespace hecate::util;

		int call_count = 0;

		// Test that references are forwarded properly
		Function<void(int&)> fn = [&call_count](int& x) {
			x += 10;
			call_count++;
		};

		int value = 5;
		fn(value);
		REQUIRE(value == 15);
		REQUIRE(call_count == 1);
	}

	TEST_CASE("function_small_object_optimization", "[hecate::util]") {
		using namespace hecate::util;

		// Small lambda should use inline storage (no heap allocation)
		int captured = 42;
		Function<int(int)> small_fn = [captured](int x) {
			return x + captured;
		};

		REQUIRE(small_fn(10) == 52);

		// Copy should also work with SBO
		Function<int(int)> small_fn_copy = small_fn;
		REQUIRE(small_fn_copy(10) == 52);
	}

	TEST_CASE("function_large_callable", "[hecate::util]") {
		using namespace hecate::util;

		// Large lambda with many captures should use heap storage
		int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8;
		Function<int(int)> large_fn = [a, b, c, d, e, f, g, h](int x) {
			return x + a + b + c + d + e + f + g + h;
		};

		REQUIRE(large_fn(10) == 46); // 10 + (1+2+3+4+5+6+7+8)
	}

	TEST_CASE("function_move_semantics", "[hecate::util]") {
		using namespace hecate::util;

		Function<int(int)> fn1 = [](int x) { return x * 2; };
		REQUIRE(fn1(5) == 10);

		Function<int(int)> fn2 = std::move(fn1);
		REQUIRE(fn2(5) == 10);
		// fn1 is now empty after move
		REQUIRE(fn1.empty());
	}

	TEST_CASE("function_concepts_constrain", "[hecate::util]") {
		using namespace hecate::util;

		// This should compile - correct signature
		Function<int(int, int)> fn1 = [](int a, int b) { return a + b; };
		REQUIRE(fn1(3, 4) == 7);

		// This should compile - convertible return type
		Function<double(int)> fn2 = [](int x) { return x * 2; };
		REQUIRE(fn2(5) == 10.0);

		// Note: The following would NOT compile due to concepts:
		// Function<int(int)> bad_fn = [](std::string s) { return 0; }; // Wrong argument type
		// Function<std::string(int)> bad_fn2 = [](int x) { return x; }; // Incompatible return type
	}
}