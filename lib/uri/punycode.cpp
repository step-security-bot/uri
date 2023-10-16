//===- lib/uri/punycode.cpp -----------------------------------------------===//
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
#include "uri/punycode.hpp"

namespace {

// returns the numeric value of a basic code point (for use in representing
// integers) in the range 0 to  base-1, or base if cp is does not represent a
// value.
constexpr std::uint_least32_t decode_digit (std::uint_least8_t cp) noexcept {
  return cp - 48 < 10   ? cp - 22
         : cp - 65 < 26 ? cp - 65
         : cp - 97 < 26 ? cp - 97
                        : uri::punycode::details::base;
}

// Decode a generalized variable-length integer.
template <typename Iterator>
std::variant<std::error_code, std::tuple<std::string::size_type, Iterator>>
decode_vli (Iterator first, Iterator last, std::string::size_type vli,
            std::string::size_type bias) {
  static constexpr auto maxint =
    std::numeric_limits<std::uint_least32_t>::max ();
  using uri::punycode::decode_error_code;
  using uri::punycode::details::base;
  using uri::punycode::details::tmax;
  using uri::punycode::details::tmin;

  std::string::size_type w = 1;
  for (auto k = base;; k += base) {
    if (first == last) {
      return make_error_code (decode_error_code::bad_input);
    }
    auto const digit = decode_digit (static_cast<std::uint_least8_t> (*first));
    ++first;

    if (digit >= base) {
      return make_error_code (decode_error_code::bad_input);
    }
    if (digit > (maxint - vli) / w) {
      return make_error_code (decode_error_code::overflow);
    }
    vli += digit * w;
    std::string::size_type const t = k <= bias          ? tmin
                                     : k >= bias + tmax ? tmax
                                                        : k - bias;
    if (digit < t) {
      break;
    }
    if (w > maxint / (base - t)) {
      return make_error_code (decode_error_code::overflow);
    }
    w *= (base - t);
  }
  return std::make_tuple (vli, first);
}

}  // end anonymous namespace

namespace uri::punycode {

char const* error_category::name () const noexcept {
  return "punycode decode";
}
std::string error_category::message (int error) const {
  auto m = "unknown error";
  switch (static_cast<decode_error_code> (error)) {
  case decode_error_code::bad_input: m = "bad input"; break;
  case decode_error_code::overflow: m = "overflow"; break;
  case decode_error_code::none: m = "unknown error"; break;
  }
  return m;
}
std::error_code make_error_code (decode_error_code const e) {
  static error_category category;
  return {static_cast<int> (e), category};
}

decode_result decode (std::string_view const& input) {
  std::u32string output;
  static constexpr auto maxint =
    std::numeric_limits<std::uint_least32_t>::max ();
  using details::adapt;
  using details::delimiter;
  using details::initial_bias;
  using details::initial_n;

  // Find the end of the literal portion (if there is one) by scanning for the
  // last delimiter.
  auto rb = std::find (input.rbegin (), input.rend (), delimiter);
  auto b = rb == input.rend () ? input.begin () : rb.base () - 1;
  std::copy (input.begin (), b, std::back_inserter (output));

  // The main decoding loop.
  auto n = initial_n;
  auto i = std::string::size_type{0};
  auto bias = initial_bias;
  // Start just after the last delimiter if any basic code points were
  // copied; start at the beginning otherwise. *in is the next character to be
  // consumed.
  auto in = b != input.begin () ? b + 1 : input.begin ();
  while (in != input.end ()) {
    // Decode a generalized variable-length integer into delta, which gets added
    // to i. The overflow checking is easier if we increase i as we go, then
    // subtract off its starting value at the end to obtain delta.
    auto const decode_res = decode_vli (in, input.end (), i, bias);
    static_assert (
      std::is_same_v<std::error_code,
                     std::remove_const_t<
                       std::variant_alternative_t<0, decltype (decode_res)>>>);
    if (auto const* err = std::get_if<std::error_code> (&decode_res)) {
      return *err;
    }
    auto const old_vli = i;
    std::tie (i, in) = std::get<1> (decode_res);
    bias = adapt (i - old_vli, output.length () + 1, old_vli == 0);

    // i was supposed to wrap around from out+1 to 0, incrementing n each time,
    // so we'll fix that now.
    if (i / (output.length () + 1) > maxint - n) {
      return make_error_code (decode_error_code::overflow);
    }
    n += i / (output.length () + 1);
    i %= (output.length () + 1);

    // Insert n into the output at position i.
    output.insert (i, std::string::size_type{1}, static_cast<char32_t> (n));
    ++i;
  }
  return output;
}

}  // end namespace uri::punycode

#if 0
  // Huge input stress test.
  constexpr auto max = static_cast<std::u32string::size_type> (std::numeric_limits<std::uint_least32_t>::max ());
  constexpr auto one = std::u32string::size_type{1};
  std::u32string input;
  input.reserve (max + one);
  for (char32_t c = 0x0001; input.length () < max + one; ++c) {
    input += c;
  }
  encode (input);
#endif

#if 0
  char32_t cp = 0;
  std::u32string in;
  for (;;) {
    auto const encoded = encode (in);
    auto const decoded = decode (encoded);
    assert (!std::holds_alternative<std::error_code>(decoded));
    assert (std::get<1> (decoded) == in);

//if (cp != delimiter) {
    in += cp;
//}
    ++cp;
  }
#endif
