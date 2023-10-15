//===- include/uri/pctencode.hpp --------------------------*- mode: C++ -*-===//
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
#ifndef URI_PCTENCODE_HPP
#define URI_PCTENCODE_HPP

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>

namespace uri {

constexpr char dec2hex (unsigned const v) noexcept {
  assert (v < 0x10);
  return static_cast<char> (v + ((v < 10) ? '0' : 'A' - 10));
}

enum class pctencode_set : std::uint8_t {
  fragment = 1U << 0U,
  query = 1U << 1U,
  special_query = 1U << 2U,
  path = 1U << 3U,
  userinfo = 1U << 4U,
  component = 1U << 5U,
  form_urlencoded =
    1U << 6U,  ///< The application/x-www-form-urlencoded percent-encode set.
};

// An implementation of section 1.3 "Percent-encoded bytes"
// https://url.spec.whatwg.org/#percent-encoded-bytes
bool needs_pctencode (std::uint_least8_t c, pctencode_set s) noexcept;

template <typename InputIterator>
bool needs_pctencode (InputIterator first, InputIterator last,
                      pctencode_set s) {
  return std::any_of (first, last,
                      [s] (auto c) { return needs_pctencode (c, s); });
}

template <typename InputIterator, typename OutputIterator>
OutputIterator pctencode (InputIterator first, InputIterator last,
                          OutputIterator out, pctencode_set encodeset) {
  for (; first != last; ++first) {
    auto c = *first;
    if (needs_pctencode (static_cast<std::uint_least8_t> (c), encodeset)) {
      auto const cu = static_cast<std::make_unsigned_t<decltype (c)>> (c);
      *(out++) = '%';
      *(out++) = dec2hex ((cu >> 4) & 0xF);
      c = dec2hex (cu & 0xF);
    }
    *(out++) = c;
  }
  return out;
}

inline std::string pctencode (std::string_view s, pctencode_set encodeset) {
  std::string result;
  result.reserve (s.length ());
  pctencode (std::begin (s), std::end (s), std::back_inserter (result),
             encodeset);
  return result;
}

}  // namespace uri
#endif  // URI_PCTENCODE_HPP
