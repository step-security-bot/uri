#include <array>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <iterator>
#include <optional>
#include <string_view>

using namespace std::literals::string_view_literals;

bool starts_with (std::string_view in, std::string_view s) {
  return in.starts_with (s);
}
//The following normalizations are described in RFC 3986 [1] to result in equivalent URIs:
//
// Converting percent-encoded triplets to uppercase. The hexadecimal digits within a percent-
// encoding triplet of the URI (e.g., %3a versus %3A) are case-insensitive and therefore should be
// normalized to use uppercase letters for the digits A-F.[2] Example:
//
//    http://example.com/foo%2a → http://example.com/foo%2A
//
// Converting the scheme and host to lowercase. The scheme and host components of the URI are case-
// insensitive and therefore should be normalized to lowercase.[3] Example:
//
//    HTTP://User@Example.COM/Foo → http://User@example.com/Foo
//
// Decoding percent-encoded triplets of unreserved characters. Percent-encoded triplets of the URI
// in the ranges of ALPHA (%41–%5A and %61–%7A), DIGIT (%30–%39), hyphen (%2D), period (%2E),
// underscore (%5F), or tilde (%7E) do not require percent-encoding and should be decoded to their
// corresponding unreserved characters.[4] Example:
//
//   http://example.com/%7Efoo → http://example.com/~foo
//
// Removing dot-segments. Dot-segments . and .. in the path component of the URI should be removed
// by applying the remove_dot_segments algorithm[5] to the path described in RFC 3986.[6] Example:
//
//   http://example.com/foo/./bar/baz/../qux → http://example.com/foo/bar/qux
//
// Converting an empty path to a "/" path. In presence of an authority component, an empty path
// component should be normalized to a path component of "/".[7] Example:
//
//   http://example.com → http://example.com/
//
// Removing the default port. An empty or default port component of the URI (port 80 for the http
// scheme) with its ":" delimiter should be removed.[8] Example:
//
//   http://example.com:80/ → http://example.com/
//
// [1]: RFC 3986, Section 6. Normalization and Comparison
// [2]: RFC 3986, Section 6.2.2.1. Case Normalization
// [3]: RFC 3986, Section 6.2.2.1. Case Normalization
// [4]: RFC 3986, Section 6.2.2.3. Path Segment Normalization
// [5]: RFC 3986, 5.2.4. Remove Dot Segments
// [6]: RFC 3986, 6.2.2.3. Path Segment Normalization
// [7]: RFC 3986, Section 6.2.3. Scheme-Based Normalization
// [8]: RFC 3986, Section 6.2.3. Scheme-Based Normalization

std::string percent_decode (std::string_view src) {
  std::string result;
  result.reserve(src.length ());
  for (auto pos = src.begin(), end = src.end (); pos != end; ++pos) {
    if (*pos == '%' && std::distance(pos, end) >= 3 && std::isxdigit (*(pos + 1)) && std::isxdigit(*(pos + 2))) {
      auto hex2dec = [] (char c) { return static_cast<unsigned> (c) - static_cast<unsigned> (std::toupper(c) >= 'A' ? 'A' + 10 : '0'); };
      result += static_cast<char> ((hex2dec(*(pos+1)) << 4) | hex2dec(*(pos+2)));
      pos += 2;
    } else {
      result += *pos;
    }
  }
  return result;
}

std::vector<std::string> remove_dot_segments (std::string_view in) {
  std::vector<std::string> out;

// 1.  The input buffer is initialized with the now-appended path
//       components and the output buffer is initialized to the empty
//       string.
//
//   2.  While the input buffer is not empty, loop as follows:
//
//       A.  If the input buffer begins with a prefix of "../" or "./",
//           then remove that prefix from the input buffer; otherwise,
//
//       B.  if the input buffer begins with a prefix of "/./" or "/.",
//           where "." is a complete path segment, then replace that
//           prefix with "/" in the input buffer; otherwise,
//
//       C.  if the input buffer begins with a prefix of "/../" or "/..",
//           where ".." is a complete path segment, then replace that
//           prefix with "/" in the input buffer and remove the last
//           segment and its preceding "/" (if any) from the output
//           buffer; otherwise,
//
//       D.  if the input buffer consists only of "." or "..", then remove
//           that from the input buffer; otherwise,
//
//       E.  move the first path segment in the input buffer to the end of
//           the output buffer, including the initial "/" character (if
//           any) and any subsequent characters up to, but not including,
//           the next "/" character or the end of the input buffer.
//
//   3.  Finally, the output buffer is returned as the result of
//       remove_dot_segments.

  if (starts_with(in, "/")) {
    in.remove_prefix (1);
  }
  while (!in.empty()) {
    if (starts_with (in, "../")) {
      in.remove_prefix (3);
    } else if (starts_with (in, "./")) {
      in.remove_prefix(2);
    } else if (starts_with(in, "/./")) {
      in.remove_prefix (3);
    } else if (in == "/.") {
      in.remove_prefix(2);
    } else if (starts_with(in, "/../")) {
      in.remove_prefix(4);
      if (!out.empty()) {
        out.pop_back();
      }
    } else if (in == "/..") {
      in.remove_prefix (3);
      if (!out.empty()) {
        out.pop_back();
      }
    } else if (in == ".") {
      in.remove_prefix(1);
    } else if (in == "..") {
      in.remove_prefix(2);
    } else {
      auto const pos = in.find('/');
      if (pos == 0) {
        in.remove_prefix(1);
      } else {
        out.emplace_back (percent_decode (in.substr (0, pos)));
        in.remove_prefix(pos != std::string::npos ? pos : in.length());
      }
    }
  }
  return out;
}






class rule {
public:
  rule () = default;
  explicit rule (std::optional<std::string_view> string) : tail_{string} {}
  rule (rule const & rhs) = default;
  rule (rule && rhs) noexcept = default;

  rule & operator= (rule const & rhs) = default;
  rule & operator= (rule && rhs) noexcept = default;

  template <typename MatchFunction, typename ActionFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule> && std::is_invocable_v<ActionFunction, std::string_view>>>
  rule concat (MatchFunction match, ActionFunction action) const {
    return concat_impl (match, action, false);
  }
  template <typename MatchFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule>>>
  rule concat (MatchFunction match) const {
    return concat(match, [] (std::string_view /*str*/) {/*nop*/});
  }

  template <typename MatchFunction, typename ActionFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule> && std::is_invocable_v<ActionFunction, std::string_view>>>
  rule concat_opt (MatchFunction match, ActionFunction action) const {
    return concat_impl (match, action, true);
  }
  template <typename MatchFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule>>>
  rule concat_opt (MatchFunction match) const {
    return concat_opt (match, [] (std::string_view /*str*/) {/*nop*/});
  }


  // Variable Repetition:  *Rule
  //
  // The operator "*" preceding an element indicates repetition.  The full form
  // is:
  //
  //   <a>*<b>element
  //
  // where <a> and <b> are optional decimal values, indicating at least <a> and
  // at most <b> occurrences of the element.
  //
  // Default values are 0 and infinity so that *<element> allows any number,
  // including zero; 1*<element> requires at least one; 3*3<element> allows
  // exactly 3 and 1*2<element> allows one or two.
  template <typename MatchFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule>>>
  rule star (MatchFunction match, unsigned min = 0, unsigned max = std::numeric_limits<unsigned>::max()) const;



  rule alternative () const { return rule{}; }
  template <typename MatchFunction, typename ...Rest>
  rule alternative (MatchFunction match, Rest && ...rest) const;


  constexpr std::optional<std::string_view> tail () const { return tail_; }

  std::optional<std::string_view> matched (rule const in) const {
    if (tail_ && in.tail_) {
      std::string_view const & intail = *in.tail_;
      return intail.substr(0, intail.length() - tail_->length());
    }
    return {};
  }

  template <typename Predicate>
  std::optional<std::string_view> single_char (Predicate pred) const;
  std::optional<std::string_view> single_char (char c) const { return single_char ([c] (char d) { return c == d; }); }

private:
  template <typename MatchFunction, typename ActionFunction>
  rule concat_impl (MatchFunction match, ActionFunction action, bool optional) const;

  std::optional<std::string_view> tail_;
};

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename ...Rest>
rule rule::alternative (MatchFunction match, Rest && ...rest) const {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (auto const m = match (*this)) {
    return rule{tail_->substr (m->length())};
  }
  // This didn't match, so try the next one.
  return this->alternative(std::forward<Rest> (rest)...);
}

// star
// ~~~~
template <typename MatchFunction, typename>
rule rule::star (MatchFunction match, unsigned min, unsigned max) const {
  if (!tail_) {
    return *this;
  }
  auto length = std::string_view::size_type{0};
  std::string_view str = *tail_;
  auto done = false;
  auto count = 0U;
  while (!done) {
    auto const m = match (rule{str});
    if (!m) {
      break; // No match so no more repetitions.
    }

    ++count;
    if (count > max) {
      return rule{}; // TODO: no match at all or just stop?
    }

    auto const l = m->length ();
    str.remove_prefix (l);
    length += l;
  }
  if (count < min) {
    return rule{};
  }
  return rule{tail_->substr(length)};
}

// concat impl
// ~~~~~~~~~~~
template <typename MatchFunction, typename ActionFunction>
rule rule::concat_impl (MatchFunction match, ActionFunction action, bool optional) const {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (std::optional<std::string_view> const m = match (*this)) {
    action (*m);
    return rule{tail_->substr (m->length())};
  }
  return optional ? *this : rule{}; // Matching failed: yield nothing or failure.
}

// single char
// ~~~~~~~~~~~
template <typename Predicate>
std::optional<std::string_view> rule::single_char (Predicate pred) const {
  auto const sv = this->tail();
  if (sv && !sv->empty () && pred(sv->front())) {
    return sv->substr(0,1);
  }
  return {};
}

auto single_char (char const first) {
  return [=] (rule r) { return r.single_char ([=](char c) { return c == first; }); };
}
auto char_range (char const first, char const last) {
  return [=] (rule r) { return r.single_char ([=](char c) { return c >= first && c <= last; }); };
}

decltype(auto) alpha (rule const r) {
  return r.single_char ([] (char const c) { return std::isalpha (static_cast<int> (c)); });
}
decltype(auto) digit (rule const r) {
  return r.single_char ([] (char c) { return std::isdigit (static_cast<int> (c)); });
}
decltype(auto) hexdig (rule const r) {
  return r.single_char ([] (char c) { return std::isxdigit (static_cast<int> (c)); });
}



decltype(auto) commercial_at (rule const r) {
  return r.single_char ('@');
}
decltype(auto) colon (rule const r) {
  return r.single_char (':');
}
decltype(auto) hash (rule const r) {
  return r.single_char ('#');
}
decltype(auto) plus (rule const r) {
  return r.single_char ('+');
}
decltype(auto) minus (rule const r) {
  return r.single_char ('-');
}
decltype(auto) solidus (rule const r) {
  return r.single_char ('/');
}
decltype(auto) question_mark (rule const r) {
  return r.single_char ('?');
}
decltype(auto) full_stop (rule const r) {
  return r.single_char ('.');
}
decltype(auto) left_square_bracket (rule const r) {
  return r.single_char ('[');
}
decltype(auto) right_square_bracket (rule const r) {
  return r.single_char (']');
}

struct uri_parts {
  void clear () {
    scheme.clear();
    userinfo.clear ();
    host.clear ();
    port.clear ();
    segments.clear ();
    query.clear ();
    fragment.clear ();
  }

  std::string scheme;
  std::string userinfo;
  std::string host;
  std::string port;
  std::vector<std::string> segments;
  std::string query;
  std::string fragment;
};


class uri {
  uri_parts result_;

  // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
  static std::optional<std::string_view> unreserved (rule const r) {
    return r.single_char ([] (char const c) {
      return std::isalnum (static_cast<int> (c)) || c == '-' || c == '.' || c == '_' || c == '~';
    });
  }
  // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
  //               / "*" / "+" / "," / ";" / "="
  static std::optional<std::string_view> sub_delims (rule const r) {
    return r.single_char ([] (char const c) {
      return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c ==  '*' || c ==  '+' || c == ',' || c == ';' || c == '=';
    });
  }
  // pct-encoded   = "%" HEXDIG HEXDIG
  static std::optional<std::string_view> pct_encoded (rule const r) {
    return r.concat (single_char ('%')).concat (hexdig).concat (hexdig).matched (r);
  }
  // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
  static std::optional<std::string_view> pchar (rule const r) {
    return r.alternative (unreserved, pct_encoded, sub_delims, colon, commercial_at).matched(r);
  }
  //userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
  static std::optional<std::string_view> userinfo (rule const r) {
    return r.star ([] (rule const r2) {
      return r2.alternative (unreserved, pct_encoded, sub_delims, colon).matched(r2);
    }).matched(r);
  }
  // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
  static std::optional<std::string_view> scheme (rule const r) {
    return r
      .concat (alpha)
      .star ([] (rule const r2) {
        return r2.alternative (alpha, digit, plus, minus, full_stop).matched(r2);
      })
      .matched(r);
  }
  // reg-name      = *( unreserved / pct-encoded / sub-delims )
  static std::optional<std::string_view> reg_name (rule const r) {
    return r.star ([] (rule const r1) {
      return r1.alternative (uri::unreserved, uri::pct_encoded, uri::sub_delims).matched(r1);
    }).matched(r);
  }
  //dec-octet     = DIGIT                 ; 0-9
  //              / %x31-39 DIGIT         ; 10-99
  //              / "1" 2DIGIT            ; 100-199
  //              / "2" %x30-34 DIGIT     ; 200-249
  //              / "25" %x30-35          ; 250-255
  static std::optional<std::string_view> dec_octet (rule const r) {
    return r.alternative(
      digit,
      [] (rule const r1) {
        return r1 // 10-99
          .concat (char_range ('1', '9')) // %x31-39
          .concat (digit)                 // DIGIT
          .matched (r1);
      },
      [] (rule const r2) {
        return r2 // 100-199
          .concat (single_char ('1')) // "1"
          .concat (digit)             // 2DIGIT
          .concat (digit)             // (...)
          .matched (r2);
      },
      [] (rule const r3) {
        return r3 // 200-249
          .concat (single_char ('2'))     // "2"
          .concat (char_range ('0', '4')) // %x30-34
          .concat (digit)                 // DIGIT
          .matched (r3);
      },
      [] (rule const r4) {
        return r4 // 250-255
          .concat (single_char ('2'))     // "2"
          .concat (single_char ('5'))     // "5"
          .concat (char_range ('0', '5')) // %x30-35
          .matched (r4);
      }
    ).matched (r);
  }
  //IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
  static std::optional<std::string_view> ipv4address (rule const r) {
    return r
      .concat (uri::dec_octet) // dec-octet
      .concat (full_stop) // "."
      .concat (uri::dec_octet) // dec-octet
      .concat (full_stop) // "."
      .concat (uri::dec_octet) // dec-octet
      .concat (full_stop) // "."
      .concat (uri::dec_octet) // dec-octet
      .matched (r);
  }
  // h16           = 1*4HEXDIG
  static std::optional<std::string_view> h16 (rule const r) { return r.star (hexdig, 1, 4).matched (r); }
  // h16colon = h16 ":"
  static std::optional<std::string_view> h16_colon (rule const r) { return r.concat (h16).concat(colon).matched(r); }
  static std::optional<std::string_view> colon_colon (rule const r) { return r.star(colon, 2, 2).matched(r); }
  // ls32          = ( h16 ":" h16 ) / IPv4address
  static std::optional<std::string_view> ls32 (rule const r) {
    return r.alternative (
      [] (rule const r1) {
        return r1.concat (uri::h16).concat (colon).concat (uri::h16).matched (r1);
      },
      uri::ipv4address
    ).matched (r);
  }
  // IPv6address   =                            6( h16 ":" ) ls32 // r1
  //               /                       "::" 5( h16 ":" ) ls32 // r2
  //               / [               h16 ] "::" 4( h16 ":" ) ls32 // r3
  //               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32 // r4
  //               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32 // r5
  //               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32 // r6
  //               / [ *4( h16 ":" ) h16 ] "::"              ls32 // r7
  //               / [ *5( h16 ":" ) h16 ] "::"              h16  // r8
  //               / [ *6( h16 ":" ) h16 ] "::"                   // r9
  static std::optional<std::string_view> ipv6address (rule const r) {
    return r.alternative (
      [] (rule const r1) { return r1.star (uri::h16_colon, 6, 6).concat (uri::ls32).matched (r1); }, // 6( h16 ":" ) ls32
      [] (rule const r2) { return r2.concat (uri::colon_colon).star (uri::h16_colon, 5, 5).concat (uri::ls32).matched (r2); }, // "::" 5( h16 ":" ) ls32
      [] (rule const r3) { return r3.concat_opt (uri::h16).concat (uri::colon_colon).star (uri::h16_colon, 4, 4).concat (uri::ls32).matched (r3); }, // [ h16 ] "::" 4( h16 ":" ) ls32
      [] (rule const r4) { return r4.concat_opt([] (rule const r4a) { return r4a.star (uri::h16_colon, 0, 1).concat (uri::h16).matched (r4a); }).concat (uri::colon_colon).star (uri::h16_colon,3,3).concat (uri::ls32).matched (r4); }, // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
      [] (rule const r5) { return r5.concat_opt([] (rule const r5a) { return r5a.star (uri::h16_colon, 0, 2).concat (uri::h16).matched (r5a); }).concat (uri::colon_colon).star (uri::h16_colon,2,2).concat (uri::ls32).matched (r5); }, // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
      [] (rule const r6) { return r6.concat_opt([] (rule const r6a) { return r6a.star (uri::h16_colon, 0, 3).concat (uri::h16).matched (r6a); }).concat (uri::colon_colon).concat (uri::h16_colon).concat (uri::ls32).matched (r6); }, // [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
      [] (rule const r7) { return r7.concat_opt([] (rule const r7a) { return r7a.star (uri::h16_colon, 0, 4).concat (uri::h16).matched (r7a); }).concat (uri::colon_colon).concat (uri::ls32).matched (r7); }, // [ *4( h16 ":" ) h16 ] "::"              ls32
      [] (rule const r8) { return r8.concat_opt([] (rule const r8a) { return r8a.star (uri::h16_colon, 0, 5).concat (uri::h16).matched (r8a); }).concat (uri::colon_colon).concat (uri::h16).matched (r8); },  // [ *5( h16 ":" ) h16 ] "::"              h16
      [] (rule const r9) { return r9.concat_opt([] (rule const r9a) { return r9a.star (uri::h16_colon, 0, 6).concat (uri::h16).matched (r9a); }).concat (uri::colon_colon).matched (r9); } // [ *6( h16 ":" ) h16 ] "::"
    ).matched(r);
  }
  // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
  static std::optional<std::string_view> ipvfuture (rule const r) {
    return r
      .concat (single_char('v'))
      .star (hexdig, 1)
      .concat (full_stop)
      .star([] (rule const r1) { return r1.alternative(uri::unreserved, uri::sub_delims, colon).matched(r1); }, 1)
      .matched (r);
  }
  //IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
  static std::optional<std::string_view> ip_literal (rule const r) {
    return r
      .concat (left_square_bracket)
      .concat ([] (rule const r2) {
        return r2.alternative (uri::ipv6address, uri::ipvfuture).matched(r2);
      })
      .concat (right_square_bracket)
      .matched (r);
  }

  auto host_rule () {
    // host          = IP-literal / IPv4address / reg-name
    return [&result=result_] (rule const r) {
      auto const host = r.alternative (
        ip_literal,
        uri::ipv4address,
        uri::reg_name
      ).matched (r);
      if (host) {
        result.host = *host;
      }
      return host;
    };
  }
  auto userinfo_at () {
    // userinfo-at = userinfo "@"
    return [&result=result_] (rule const r) {
      return r
        .concat (uri::userinfo, [&result] (std::string_view const userinfo) { result.userinfo = userinfo; })
        .concat (commercial_at)
        .matched (r);
    };
  }

  // port = *DIGIT
  static std::optional<std::string_view> port (rule const r) {
    return r.star(digit).matched(r);
  }

  auto colon_port () {
    // colon-port = ":" port
    return [&result=result_] (rule const r) {
      return r
        .concat (colon)
        .concat (port, [&result] (std::string_view const p) { result.port = p; })
        .matched (r);
    };
  }

  auto authority () {
    //authority = [ userinfo "@" ] host [ ":" port ]
    return [this] (rule const r) {
      return r
        .concat_opt (this->userinfo_at ())
        .concat (this->host_rule ())
        .concat_opt (this->colon_port ())
        .matched(r);
    };
  }

  //segment       = *pchar
  static std::optional<std::string_view> segment (rule const r) {
    return r.star (uri::pchar).matched (r);
  }
  // segment-nz    = 1*pchar
  static std::optional<std::string_view> segment_nz (rule const r) {
    return r.star (uri::pchar, 1U).matched (r);
  }
  // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
  //                  ; non-zero-length segment without any colon ":"
  static std::optional<std::string_view> segment_nz_nc (rule const r) {
    return r.star ([] (rule const r2) {
      return r2.alternative(uri::unreserved, uri::pct_encoded, uri::sub_delims, commercial_at).matched(r2);
    }, 1U)
    .matched(r);
  }
  auto path_abempty () {
    // path-abempty  = *( "/" segment )
    return [&result=result_] (rule const r) {
      return r.star ([&result] (rule const r2) {
        return r2.concat(solidus).concat(segment, [&result] (std::string_view const seg) {
          result.segments.emplace_back (seg);
        }).matched(r2);
      }).matched(r);
    };
  }
  auto path_absolute () {
    //path-absolute = "/" [ segment-nz *( "/" segment ) ]
    return [&result=result_] (rule const r) {
      return r
        .concat (solidus)
        .concat_opt ([&result] (rule const r1) {
          return r1
            .concat (uri::segment_nz)
            .concat ([&result] (rule const r2) {
              return r2.star ([&result] (rule const r3) {
                return r3.concat (solidus).concat(segment, [&result] (std::string_view const seg) {
                  result.segments.emplace_back (seg);
                }).matched(r3);
              })
              .matched(r2);
            })
            .matched(r1);
        })
        .matched(r);
    };
  }
  //path-empty    = 0<pchar>
  static std::optional<std::string_view> path_empty (rule const r) {
    return r.star(uri::pchar, 0, 0).matched(r);
  }
  //path-rootless = segment-nz *( "/" segment )
  auto path_rootless () {
    return [&result=result_] (rule const r) {
      return r
        .concat (uri::segment_nz)
        .star ([&result] (rule const r1) {
          return r1
            .concat(solidus)
            .concat(segment, [&result] (std::string_view const seg) {
              result.segments.emplace_back (seg);
            })
            .matched (r1);
        }).matched (r);
    };
  }

  // hier-part     = "//" authority path-abempty
  //               / path-absolute
  //               / path-rootless
  //               / path-empty
  auto hier_part () {
    return [this] (rule const r) {
      return r
        .alternative([this] (rule const r1) {
          return r1
            .concat (solidus)
            .concat (solidus)
            .concat (this->authority ())
            .concat (this->path_abempty ())
            .matched (r1);
        }, this->path_rootless (), this->path_absolute (), path_empty)
        .matched (r);
    };
  }

  // query         = *( pchar / "/" / "?" )
  static std::optional<std::string_view> query (rule const r) {
    return r.star ([] (rule const r2) {
      return r2.alternative (uri::pchar, solidus, question_mark).matched(r2);
    }).matched(r);
  }
  // fragment      = *( pchar / "/" / "?" )
  static std::optional<std::string_view> fragment (rule const r) {
    return query (r);
  }
public:
  // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
  std::optional<uri_parts> uri_rule (std::string_view const in) {
    bool success = rule{in}
      .concat (uri::scheme, [&result=result_] (std::string_view const scheme) {
        result.scheme = scheme;
      })
      .concat (colon)
      .concat (this->hier_part ())
      .concat_opt ([&result=result_] (rule const rq) {
        // "?" query
        return rq
          .concat (question_mark) // "?"
          .concat (uri::query, [&result] (std::string_view const query) { // query
            result.query = query;
          })
          .matched (rq);
      })
      .concat_opt ([&result=result_] (rule const rf) {
        // "#" fragment
        return rf
          .concat (hash) // "#"
          .concat (uri::fragment, [&result] (std::string_view const fragment) { // fragment
            result.fragment = fragment;
          })
          .matched (rf);
      })
      .tail ()
      .has_value ();
    return success ? std::optional<uri_parts>{result_} : std::optional<uri_parts>{std::nullopt};
  }

  void clear () {
    result_.clear ();
  }
  uri_parts const & result () const { return result_; }
};

int main() {
  //auto const uri = "https://userinfo@foo.com:443/ab/./cd/../foo%20bar?query#fragment"sv;
  //auto const uri = "https://foo.com//ab/./cd/../foo%20bar"sv;
  auto const str = "https://userinfo@127.0.0.1:443/ab/./cd/../foo%20bar?query#fragment"sv;
  uri x;
  auto r = x.uri_rule (str);

  std::cout << "URI: " << str << '\n';
  if (!r) {
    return EXIT_FAILURE;
  }
  std::cout << "scheme: " << std::quoted (r->scheme) << '\n'
            << "userinfo: " << std::quoted (r->userinfo) << '\n'
            << "host: " << std::quoted (r->host) << '\n'
            << "port: " << std::quoted (r->port) << '\n';
  std::string path;
  for (auto const & s: r->segments) {
    path += '/';
    path += s;
  }
  std::cout << "path: " << std::quoted (path) << '\n';
  std::cout << "query: " << std::quoted (r->query) << '\n';
  std::cout << "fragment: " << std::quoted (r->fragment) << '\n';
  return EXIT_SUCCESS;
}

#if 0
  std::vector<std::string> out = remove_dot_segments (uri);
  std::string result;
  for (auto const & s: out) {
    result += '/';
    result += s;
  }
  std::cout << std::quoted(result) << '\n';
#endif
