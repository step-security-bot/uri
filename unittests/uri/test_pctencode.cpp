//===- unittests/uri/test_pctencode.cpp -----------------------------------===//
//*             _                           _       *
//*  _ __   ___| |_ ___ _ __   ___ ___   __| | ___  *
//* | '_ \ / __| __/ _ \ '_ \ / __/ _ \ / _` |/ _ \ *
//* | |_) | (__| ||  __/ | | | (_| (_) | (_| |  __/ *
//* | .__/ \___|\__\___|_| |_|\___\___/ \__,_|\___| *
//* |_|                                             *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
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

// NOLINTNEXTLINE
TEST (PctEncode, Hello) {
  auto input = "Hello"sv;
  std::string out;
  uri::pctencode (std::begin (input), std::end (input),
                  std::back_inserter (out));
  EXPECT_EQ (out, "Hello");
}

// NOLINTNEXTLINE
TEST (PctEncode, Percent) {
  auto input = "H%llo"sv;
  std::string out;
  uri::pctencode (std::begin (input), std::end (input),
                  std::back_inserter (out));
  EXPECT_EQ (out, "H%25llo");
}

#if URI_FUZZTEST
static void pctencode_never_crashes (std::string const& s) {
  std::string encoded;
  uri::pctencode (std::begin (s), std::end (s), std::back_inserter (encoded));
}
FUZZ_TEST (PctEncodeFuzz, pctencode_never_crashes);

static void pctcodec_equals_original (std::string const& s) {
  std::string encoded;
  uri::pctencode (std::begin (s), std::end (s), std::back_inserter (encoded));

  std::string out;
  std::copy (uri::pctdecode_begin (encoded), uri::pctdecode_end (encoded),
             std::back_inserter (out));

  EXPECT_THAT (out, testing::StrEq (s));
}
FUZZ_TEST (PctEncodeFuzz, pctcodec_equals_original);
#endif  // URI_FUZZTEST
