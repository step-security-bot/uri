//===- include/uri/punycode.hpp ---------------------------*- mode: C++ -*-===//
//*                                        _       *
//*  _ __  _   _ _ __  _   _  ___ ___   __| | ___  *
//* | '_ \| | | | '_ \| | | |/ __/ _ \ / _` |/ _ \ *
//* | |_) | |_| | | | | |_| | (_| (_) | (_| |  __/ *
//* | .__/ \__,_|_| |_|\__, |\___\___/ \__,_|\___| *
//* |_|                |___/                       *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
#ifndef URI_PUNYCODE_HPP
#define URI_PUNYCODE_HPP

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string_view>
#include <system_error>
#include <tuple>
#include <variant>

namespace uri::punycode {

enum class decode_error_code : int {
  none,
  bad_input,
  overflow,
};

class error_category final : public std::error_category {
public:
  char const* name () const noexcept override;
  std::string message (int error) const override;
};
std::error_code make_error_code (decode_error_code const e);

namespace details {

constexpr auto damp = 700U;
constexpr auto base = 36U;
constexpr auto tmin = 1U;
constexpr auto tmax = 26U;
constexpr auto skew = 38U;
constexpr auto initial_bias = std::string::size_type{72};
constexpr auto initial_n = std::string::size_type{0x80};
constexpr auto delimiter = char{0x2D};

constexpr bool is_basic_code_point (char32_t c) noexcept {
  // return c >= 0x20 && c <= 0x7E;
  return c <= 0x7F;
}

// returns the basic code point whose value (when used for representing
// integers) is d, which needs to be in the range 0 to base-1. The lowercase
// form is used unless flag is nonzero, in which case the uppercase form is
// used. The behavior is undefined if flag is nonzero and digit d has no
// uppercase form.
constexpr char encode_digit (std::string::size_type d) noexcept {
  //  0..25 maps to ASCII a..z; 26..35 maps to ASCII 0..9
  return static_cast<char> (d + 22U + 75U * (d < 26U));
}

constexpr std::string::size_type clamp (std::string::size_type k,
                                        std::string::size_type bias) {
  if (k <= bias) {
    return tmin;
  }
  if (k >= bias + tmax) {
    return tmax;
  }
  return k - bias;
}

template <typename OutputIterator>
OutputIterator encode_vli (std::string::size_type q,
                           std::string::size_type bias, OutputIterator out) {
  for (auto k = base;; k += base) {
    auto const t = clamp (k, bias);
    if (q < t) {
      break;
    }
    *(out++) = encode_digit (t + (q - t) % (base - t));
    q = (q - t) / (base - t);
  }
  *(out++) = encode_digit (q);
  return out;
}

constexpr std::string::size_type adapt (std::string::size_type delta,
                                        std::size_t numpoints, bool firsttime) {
  delta = firsttime ? delta / damp : delta >> 1U;
  delta += delta / numpoints;
  auto k = 0U;
  while (delta > ((base - tmin) * tmax) / 2U) {
    delta = delta / (base - tmin);
    k += base;
  }
  return k + (base - tmin + 1) * delta / (delta + skew);
}

}  // namespace details

template <typename OutputIterator>
OutputIterator encode (std::u32string_view const& input,
                       OutputIterator output) {
  std::u32string nonbasic;
  std::string::size_type num_basics = 0;
  // Handle the basic code points. Copy them to the output in order followed by
  // a delimiter if any were copied.
  for (auto cp : input) {
    if (details::is_basic_code_point (cp)) {
      *(output++) = static_cast<char> (cp);
      ++num_basics;
    } else {
      nonbasic += cp;
    }
  }
  {
    // Sort and remove duplicates.
    auto first = nonbasic.begin ();
    auto last = nonbasic.end ();
    std::sort (first, last);
    nonbasic.erase (std::unique (first, last), last);
  }
  auto i = num_basics;
  if (num_basics > 0) {
    *(output++) = details::delimiter;
  }
  auto n = details::initial_n;
  auto delta = std::string::size_type{0};
  auto bias = details::initial_bias;
  for (char32_t const m : nonbasic) {
    assert (m >= n);
    delta += (m - n) * (i + 1);
    n = m;
    for (char32_t const c :
         input) {  // for each code point c in the input (in order)
      if (c < n) {
        ++delta;  // increment delta (fail on overflow)
      } else if (c == n) {
        // Represent delta as a generalized variable-length integer.
        output = details::encode_vli (delta, bias, output);
        bias = details::adapt (delta, i + 1, i == num_basics);
        delta = 0U;
        ++i;
      }
    }
    ++delta;
    ++n;
  }
  return output;
}

using decode_result = std::variant<std::error_code, std::u32string>;
decode_result decode (std::string_view const& input);

}  // end namespace uri::punycode

#endif  // URI_PUNYCODE_HPP
