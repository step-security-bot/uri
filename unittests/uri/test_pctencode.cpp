#include <iterator>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "uri/pctdecode.hpp"
#include "uri/pctencode.hpp"

#if URI_FUZZTEST
#include "fuzztest/fuzztest.h"
#endif

using namespace std::string_view_literals;

TEST (PctEncode, Hello) {
  auto input = "Hello"sv;
  std::string out;
  uri::pctencode (std::begin (input), std::end (input),
                  std::back_inserter (out));
  EXPECT_EQ (out, "Hello");
}

static void pctcodec_equals_original (std::string const& s) {
  std::string encoded;
  uri::pctencode (std::begin (s), std::end (s), std::back_inserter (encoded));

  std::string out;
  std::copy (uri::pctdecode_begin (encoded), uri::pctdecode_end (encoded),
             std::back_inserter (out));

  EXPECT_THAT (out, testing::StrEq (s));
}
#if URI_FUZZTEST
FUZZ_TEST (TagUtilsFuzzTests, pctcodec_equals_original);
#endif

TEST (TagUtilsFuzzTests, pctcodec_equals_originalRegression) {
  pctcodec_equals_original ("");
}
