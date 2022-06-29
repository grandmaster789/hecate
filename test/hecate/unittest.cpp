#include <catch2/catch_test_macros.hpp>

#include "hecate.h"

TEST_CASE("Trivial test", "[hecate]") {
  int x = hecate::square(5);
  REQUIRE(x == 25);
}