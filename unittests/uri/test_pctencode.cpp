#include <gtest/gtest.h>

#include <iterator>
#include <string_view>

#include "uri/pctencode.hpp"

using namespace std::string_view_literals;

TEST (PctEncode, Hello) {
  auto input = "Hello"sv;
  std::string out;
  uri::pctencode (std::begin (input), std::end (input),
                  std::back_inserter (out));
  EXPECT_EQ (out, "Hello");
}
