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
  using value_type = typename std::iterator_traits<InputIterator>::value_type;
  static std::array<value_type, 18> const delims{{
    ':', '/', '?', '#', '[', ']', '@',                      // gen-delims
    '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='  // sub-delims
  }};
  auto const begin = std::begin (delims);
  auto const end = std::end (delims);

  for (; first != last; ++first) {
    auto c = *first;
    auto cu = static_cast<std::make_unsigned_t<decltype (c)>> (c);
    if (cu > 0x7F || cu < 0x20 || std::find (begin, end, c) != end) {
      *(out++) = '%';
      *(out++) = dec2hex ((cu >> 4) & 0xF);
      c = dec2hex (cu & 0xF);
    }
    *(out++) = c;
  }
  return out;
}

}  // namespace uri
#endif  // URI_PCTENCODE_HPP
