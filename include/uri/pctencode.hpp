//===- include/uri/pctencode.hpp --------------------------*- mode: C++ -*-===//
//*             _                           _       *
//*  _ __   ___| |_ ___ _ __   ___ ___   __| | ___  *
//* | '_ \ / __| __/ _ \ '_ \ / __/ _ \ / _` |/ _ \ *
//* | |_) | (__| ||  __/ | | | (_| (_) | (_| |  __/ *
//* | .__/ \___|\__\___|_| |_|\___\___/ \__,_|\___| *
//* |_|                                             *
//===----------------------------------------------------------------------===//
//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
// for license information.
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//
#ifndef URI_PCTENCODE_HPP
#define URI_PCTENCODE_HPP

#include <array>
#include <cassert>
#include <string>

namespace uri {

constexpr char dec2hex (unsigned const v) noexcept {
  assert (v < 0x10);
  return static_cast<char> (v + ((v < 10) ? '0' : 'A' - 10));
}

// punctuation characters. This collection of characters is based on the "C"
// locale's "printable" punctuation characters with the set of gen-delims and
// sub-delims characters from the URI specification removed.
template <typename ValueT>
inline constexpr std::array<ValueT, 14> delims{
  {'"', '-', '.', '<', '>', '\\', '^', '_', '`', '{', '|', '}', '~', ')'}};

template <typename ValueT>
constexpr bool isprint (ValueT const v) {
  if (v >= '0' && v <= '9') {
    return true;
  }
  if (v >= 'A' && v <= 'Z') {
    return true;
  }
  if (v >= 'a' && v <= 'z') {
    return true;
  }
  if (v == ' ') {
    return true;
  }
  constexpr auto end = std::end (delims<ValueT>);
  return std::find (std::begin (delims<ValueT>), end, v) != end;
}

template <typename InputIterator>
bool needs_pctencode (InputIterator first, InputIterator last) {
  return std::any_of (first, last, [] (auto c) { return !isprint (c); });
}

// punctuation characters. This collection of characters is based on the "C"
// locale's "printable" punctuation characters with the set of gen-delims and
// sub-delims characters from the URI specification removed.
template <typename ValueT>
inline constexpr std::array<ValueT, 14> delims{
  {'"', '-', '.', '<', '>', '\\', '^', '_', '`', '{', '|', '}', '~', ')'}};

template <typename ValueT>
constexpr bool isprint (ValueT const v) {
  if (v >= '0' && v <= '9') {
    return true;
  }
  if (v >= 'A' && v <= 'Z') {
    return true;
  }
  if (v >= 'a' && v <= 'z') {
    return true;
  }
  if (v == ' ') {
    return true;
  }
  constexpr auto end = std::end (delims<ValueT>);
  return std::find (std::begin (delims<ValueT>), end, v) != end;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator pctencode (InputIterator first, InputIterator last,
                          OutputIterator out) {
  for (; first != last; ++first) {
    auto c = *first;
    if (!isprint (c)) {
      auto const cu = static_cast<std::make_unsigned_t<decltype (c)>> (c);
      *(out++) = '%';
      *(out++) = dec2hex ((cu >> 4) & 0xF);
      c = dec2hex (cu & 0xF);
    }
    *(out++) = c;
  }
  return out;
}

inline std::string pctencode (std::string_view s) {
  std::string result;
  result.reserve (s.length ());
  pctencode (std::begin (s), std::end (s), std::back_inserter (result));
  return result;
}

}  // namespace uri
#endif  // URI_PCTENCODE_HPP
