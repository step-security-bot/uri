#ifndef URI_PCTENCODE_HPP
#define URI_PCTENCODE_HPP

#include <cassert>
#include <string>

namespace uri {

constexpr char dec2hex (unsigned const v) noexcept {
  assert (v < 0x10);
  return static_cast<char> (v + ((v < 10) ? '0' : 'a' - 10));
}

template <typename InputIterator, typename OutputIterator>
OutputIterator pctencode (InputIterator first, InputIterator last,
                          OutputIterator out) {
  static std::u8string const gen_delims = u8":/?#[]@";
  static std::u8string const sub_delims = u8"!$&'()*+,;=";

  for (; first != last; ++first) {
    auto c = *first;
    if (c > 0x7F || c < 0x20 || gen_delims.find (c) != std::string::npos ||
        sub_delims.find (c) != std::string::npos) {
      *(out++) = '%';
      *(out++) = dec2hex ((c >> 4) & 0xF);
      c = dec2hex (c & 0xF);
    }
    *(out++) = c;
  }
  return out;
}

}  // namespace uri
#endif  // URI_PCTENCODE_HPP
