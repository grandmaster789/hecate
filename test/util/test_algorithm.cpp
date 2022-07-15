#include "../unittest.h"

#include "hecate.h"

#include <vector>

namespace test {
	TEST_CASE("consume_if", "[hecate::util]") {
		std::vector<int> data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> out;

		hecate::util::consume_if(
			data, 
			[]    (int value) { return value % 2 == 0; }, 
			[&out](int value) { out.push_back(value); }
		);

		std::vector<int> expected_data = { 1, 3, 5, 7, 9  };
		std::vector<int> expected_out  = { 2, 4, 6, 8, 10 };

		REQUIRE(data == expected_data);
		REQUIRE(out == expected_out);
	}
}