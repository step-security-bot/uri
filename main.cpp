#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

// URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
//
// hier-part     = "//" authority path-abempty
//               / path-absolute
//               / path-rootless
//               / path-empty
//
// URI-reference = URI / relative-ref
//
// absolute-URI  = scheme ":" hier-part [ "?" query ]
//
// relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
//
// relative-part = "//" authority path-abempty
//               / path-absolute
//               / path-noscheme
//               / path-empty
//
// scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
//
// authority     = [ userinfo "@" ] host [ ":" port ]
// userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
// host          = IP-literal / IPv4address / reg-name
// port          = *DIGIT
//
// IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
//
// IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
//
// IPv6address   =                            6( h16 ":" ) ls32
//               /                       "::" 5( h16 ":" ) ls32
//               / [               h16 ] "::" 4( h16 ":" ) ls32
//               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
//               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
//               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
//               / [ *4( h16 ":" ) h16 ] "::"              ls32
//               / [ *5( h16 ":" ) h16 ] "::"              h16
//               / [ *6( h16 ":" ) h16 ] "::"
//
// h16           = 1*4HEXDIG
// ls32          = ( h16 ":" h16 ) / IPv4address
// IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
// dec-octet     = DIGIT                 ; 0-9
//               / %x31-39 DIGIT         ; 10-99
//               / "1" 2DIGIT            ; 100-199
//               / "2" %x30-34 DIGIT     ; 200-249
//               / "25" %x30-35          ; 250-255
//
// reg-name      = *( unreserved / pct-encoded / sub-delims )
//
// path          = path-abempty    ; begins with "/" or is empty
//               / path-absolute   ; begins with "/" but not "//"
//               / path-noscheme   ; begins with a non-colon segment
//               / path-rootless   ; begins with a segment
//               / path-empty      ; zero characters
//
// path-abempty  = *( "/" segment )
// path-absolute = "/" [ segment-nz *( "/" segment ) ]
// path-noscheme = segment-nz-nc *( "/" segment )
// path-rootless = segment-nz *( "/" segment )
// path-empty    = 0<pchar>
//
// segment       = *pchar
// segment-nz    = 1*pchar
// segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
//                  ; non-zero-length segment without any colon ":"
//
// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
//
// query         = *( pchar / "/" / "?" )
//
// fragment      = *( pchar / "/" / "?" )
//
// pct-encoded   = "%" HEXDIG HEXDIG
//
// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
// reserved      = gen-delims / sub-delims
// gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
// sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//               / "*" / "+" / "," / ";" / "="

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;

bool starts_with (std::string_view in, std::string_view s) {
  return in.starts_with (s);
}
// The following normalizations are described in RFC 3986 [1] to result in
// equivalent URIs:
//
//  Converting percent-encoded triplets to uppercase. The hexadecimal digits
//  within a percent- encoding triplet of the URI (e.g., %3a versus %3A) are
//  case-insensitive and therefore should be normalized to use uppercase letters
//  for the digits A-F.[2] Example:
//
//     http://example.com/foo%2a → http://example.com/foo%2A
//
//  Converting the scheme and host to lowercase. The scheme and host components
//  of the URI are case- insensitive and therefore should be normalized to
//  lowercase.[3] Example:
//
//     HTTP://User@Example.COM/Foo → http://User@example.com/Foo
//
//  Decoding percent-encoded triplets of unreserved characters. Percent-encoded
//  triplets of the URI in the ranges of ALPHA (%41–%5A and %61–%7A), DIGIT
//  (%30–%39), hyphen (%2D), period (%2E), underscore (%5F), or tilde (%7E) do
//  not require percent-encoding and should be decoded to their corresponding
//  unreserved characters.[4] Example:
//
//    http://example.com/%7Efoo → http://example.com/~foo
//
//  Removing dot-segments. Dot-segments . and .. in the path component of the
//  URI should be removed by applying the remove_dot_segments algorithm[5] to
//  the path described in RFC 3986.[6] Example:
//
//    http://example.com/foo/./bar/baz/../qux → http://example.com/foo/bar/qux
//
//  Converting an empty path to a "/" path. In presence of an authority
//  component, an empty path component should be normalized to a path component
//  of "/".[7] Example:
//
//    http://example.com → http://example.com/
//
//  Removing the default port. An empty or default port component of the URI
//  (port 80 for the http scheme) with its ":" delimiter should be removed.[8]
//  Example:
//
//    http://example.com:80/ → http://example.com/
//
//  [1]: RFC 3986, Section 6. Normalization and Comparison
//  [2]: RFC 3986, Section 6.2.2.1. Case Normalization
//  [3]: RFC 3986, Section 6.2.2.1. Case Normalization
//  [4]: RFC 3986, Section 6.2.2.3. Path Segment Normalization
//  [5]: RFC 3986, 5.2.4. Remove Dot Segments
//  [6]: RFC 3986, 6.2.2.3. Path Segment Normalization
//  [7]: RFC 3986, Section 6.2.3. Scheme-Based Normalization
//  [8]: RFC 3986, Section 6.2.3. Scheme-Based Normalization

std::string percent_decode (std::string_view src) {
  std::string result;
  result.reserve (src.length ());
  for (auto pos = src.begin (), end = src.end (); pos != end; ++pos) {
    if (*pos == '%' && std::distance (pos, end) >= 3 &&
        std::isxdigit (*(pos + 1)) && std::isxdigit (*(pos + 2))) {
      auto hex2dec = [] (char c) {
        return static_cast<unsigned> (c) -
               static_cast<unsigned> (std::toupper (c) >= 'A' ? 'A' + 10 : '0');
      };
      result += static_cast<char> ((hex2dec (*(pos + 1)) << 4) |
                                   hex2dec (*(pos + 2)));
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

  if (starts_with (in, "/")) {
    in.remove_prefix (1);
  }
  while (!in.empty ()) {
    if (starts_with (in, "../")) {
      in.remove_prefix (3);
    } else if (starts_with (in, "./")) {
      in.remove_prefix (2);
    } else if (starts_with (in, "/./")) {
      in.remove_prefix (3);
    } else if (in == "/.") {
      in.remove_prefix (2);
    } else if (starts_with (in, "/../")) {
      in.remove_prefix (4);
      if (!out.empty ()) {
        out.pop_back ();
      }
    } else if (in == "/..") {
      in.remove_prefix (3);
      if (!out.empty ()) {
        out.pop_back ();
      }
    } else if (in == ".") {
      in.remove_prefix (1);
    } else if (in == "..") {
      in.remove_prefix (2);
    } else {
      auto const pos = in.find ('/');
      if (pos == 0) {
        in.remove_prefix (1);
      } else {
        out.emplace_back (percent_decode (in.substr (0, pos)));
        in.remove_prefix (pos != std::string::npos ? pos : in.length ());
      }
    }
  }
  return out;
}

template <typename T>
std::vector<T> join (std::vector<T> const& a, std::vector<T> const& b) {
  std::vector<T> res;
  res.reserve (a.size () + b.size ());
  res.insert (res.end (), a.begin (), a.end ());
  res.insert (res.end (), b.begin (), b.end ());
  return res;
}

class rule {
public:
  using optsv = std::optional<std::string_view>;
  using acceptor = std::function<void (std::string_view)>;
  using acceptor_container =
    std::vector<std::tuple<acceptor, std::string_view>>;

private:
  rule (optsv string, acceptor_container&& acceptors)
      : tail_{string}, acceptors_{std::move (acceptors)} {}
  rule (optsv string, acceptor_container const& acceptors)
      : tail_{string}, acceptors_{acceptors} {}
  rule () = default;

public:
  explicit rule (std::string_view string) : tail_{string} {}
  rule (rule const& rhs) = default;
  rule (rule&& rhs) noexcept = default;
  ~rule () noexcept = default;

  rule& operator= (rule const& rhs) = default;
  rule& operator= (rule&& rhs) noexcept = default;

  bool done () {
    if (!tail_ || !tail_->empty ()) {
      return false;
    }
    std::for_each (std::begin (acceptors_), std::end (acceptors_),
                   [] (acceptor_container::value_type const& a) {
                     std::get<0> (a) (std::get<1> (a));
                   });
    return true;
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule concat (MatchFunction match, AcceptFunction accept) {
    return concat_impl (match, accept, false);
  }

  static void accept_nop (std::string_view) { /* do nothing */
  }

  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule concat (MatchFunction match) {
    return concat (match, rule::accept_nop);
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule optional (MatchFunction match, AcceptFunction accept) {
    if (!tail_) {
      // If matching has already failed, then pass that condition down the
      // chain.
      return *this;
    }
    rule res = rule{*tail_}.concat_impl (match, accept, true);
    if (!res.tail_) {
      return *this;  // The rule failed, so carry on as if nothing happened.
    }
    return rule{res.tail_, join (acceptors_, res.acceptors_)};
  }
  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule optional (MatchFunction match) {
    return optional (match, rule::accept_nop);
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
  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule star (MatchFunction match, unsigned min = 0,
             unsigned max = std::numeric_limits<unsigned>::max ());

  rule alternative () { return rule{}; }
  template <typename MatchFunction, typename... Rest>
  rule alternative (MatchFunction match, Rest&&... rest);

  constexpr optsv tail () const { return tail_; }

  using matched_result =
    std::optional<std::tuple<std::string_view, acceptor_container>>;

  matched_result matched (char const* name, rule const& in) const {
    assert (!tail_ || in.tail_);
    static constexpr auto trace = false;
    if (trace) {
      std::cout << ((tail_ && in.tail_) ? '*' : '-') << ' '
                << std::quoted (name);
    }

    if (tail_ && in.tail_) {
      std::string_view const& intail = *in.tail_;
      std::string_view const str =
        intail.substr (0, intail.length () - tail_->length ());
      if (trace) {
        std::cout << ' ' << std::quoted (str) << '\n';
      }
      return std::make_tuple (str, acceptors_);
    }

    if (trace) {
      std::cout << '\n';
    }
    return {};
  }

  template <typename Predicate>
  matched_result single_char (Predicate pred) const;
  matched_result single_char (char const c) const {
    return single_char ([c] (char d) { return c == d; });
  }

private:
  template <typename MatchFunction, typename AcceptFunction>
  rule concat_impl (MatchFunction match, AcceptFunction accept, bool optional);

  rule alternative_impl () { return rule{}; }
  template <typename MatchFunction, typename... Rest>
  rule alternative_impl (MatchFunction match, Rest&&... rest);

  rule join_rule (matched_result::value_type const& m);

  acceptor_container acceptors_;
  optsv tail_;
};

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename... Rest>
rule rule::alternative (MatchFunction match, Rest&&... rest) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  auto r = rule{*tail_}.alternative_impl (match, std::forward<Rest> (rest)...);
  if (!r.tail_) {
    return rule{};
  }
  return rule{r.tail_, join (acceptors_, r.acceptors_)};
}

rule rule::join_rule (matched_result::value_type const& m) {
  auto const& [head, acceptors] = m;
  return {tail_->substr (head.length ()), join (acceptors_, acceptors)};
}

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename... Rest>
rule rule::alternative_impl (MatchFunction match, Rest&&... rest) {
  if (matched_result const m = match (*this)) {
    return join_rule (*m);
  }
  // This didn't match, so try the next one.
  return this->alternative_impl (std::forward<Rest> (rest)...);
}

// star
// ~~~~
template <typename MatchFunction, typename>
rule rule::star (MatchFunction const match, unsigned const min,
                 unsigned const max) {
  if (!tail_) {
    return *this;
  }
  auto length = std::string_view::size_type{0};
  std::string_view str = *tail_;
  auto count = 0U;
  // acceptor_container acceptors = acceptors_;
  for (;;) {
    matched_result const m = match (rule{str});
    if (!m) {
      break;  // No match so no more repetitions.
    }
    ++count;
    if (count > max) {
      return rule{};
    }
    auto const l = std::get<std::string_view> (*m).length ();
    str.remove_prefix (l);
    length += l;

    auto const& a = std::get<acceptor_container> (*m);
    acceptors_.insert (acceptors_.end (), a.begin (), a.end ());
  }
  if (count < min) {
    return rule{};
  }

  return rule{tail_->substr (length), acceptors_};
}

// concat impl
// ~~~~~~~~~~~
template <typename MatchFunction, typename AcceptFunction>
rule rule::concat_impl (MatchFunction match, AcceptFunction accept,
                        bool optional) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (matched_result m = match (rule{*tail_})) {
    std::get<acceptor_container> (*m).emplace_back (
      accept, std::get<std::string_view> (*m));
    return join_rule (*m);
  }
  if (optional) {
    return *this;
  }
  return rule{};  // Matching failed: yield nothing or failure.
}

// single char
// ~~~~~~~~~~~
template <typename Predicate>
auto rule::single_char (Predicate const pred) const -> matched_result {
  auto const sv = this->tail ();
  if (sv && !sv->empty () && pred (sv->front ())) {
    return std::make_tuple (sv->substr (0, 1), acceptor_container{});
  }
  return {};
}

auto single_char (char const first) {
  return [=] (rule const r) {
    return r.single_char ([=] (char const c) { return c == first; });
  };
}
auto char_range (char const first, char const last) {
  return [=] (rule const r) {
    return r.single_char (
      [=] (char const c) { return c >= first && c <= last; });
  };
}

auto alpha (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isalpha (static_cast<int> (c)); });
}
auto digit (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isdigit (static_cast<int> (c)); });
}
auto hexdig (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isxdigit (static_cast<int> (c)); });
}

auto commercial_at (rule const r) {
  return r.single_char ('@');
}
auto colon (rule const r) {
  return r.single_char (':');
}
auto hash (rule const r) {
  return r.single_char ('#');
}
auto plus (rule const r) {
  return r.single_char ('+');
}
auto minus (rule const r) {
  return r.single_char ('-');
}
auto solidus (rule const r) {
  return r.single_char ('/');
}
auto question_mark (rule const r) {
  return r.single_char ('?');
}
auto full_stop (rule const r) {
  return r.single_char ('.');
}
auto left_square_bracket (rule const r) {
  return r.single_char ('[');
}
auto right_square_bracket (rule const r) {
  return r.single_char (']');
}

struct uri_parts {
  void clear () {
    scheme.reset ();
    userinfo.reset ();
    host.reset ();
    port.reset ();
    segments.clear ();
    query.reset ();
    fragment.reset ();
  }

  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  std::vector<std::string> segments;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

auto single_colon (rule r) -> rule::matched_result {
  return r.concat (colon)
    .concat ([] (rule const& r2) -> rule::matched_result {
      auto const& sv = r2.tail ();
      if (!sv) {
        return {};
      }
      if (sv->empty () || sv->front () != ':') {
        return std::make_tuple (sv->substr (0, 0), rule::acceptor_container{});
      }
      if (sv->front () != ':') {
        return std::make_tuple (sv->substr (0, 1), rule::acceptor_container{});
      }
      return {};
    })
    .matched ("single-colon", r);
}

class uri {
  // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
  //               / "*" / "+" / "," / ";" / "="
  static auto sub_delims (rule const r) {
    return r.single_char ([] (char const c) {
      return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
             c == ')' || c == '*' || c == '+' || c == ',' || c == ';' ||
             c == '=';
    });
  }
  // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
  static auto unreserved (rule r) {
    return r.single_char ([] (char const c) {
      return std::isalnum (static_cast<int> (c)) || c == '-' || c == '.' ||
             c == '_' || c == '~';
    });
  }
  // pct-encoded   = "%" HEXDIG HEXDIG
  static auto pct_encoded (rule r) {
    return r.concat (single_char ('%'))
      .concat (hexdig)
      .concat (hexdig)
      .matched ("pct-encoded", r);
  }
  // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
  static auto pchar (rule r) {
    return r
      .alternative (unreserved, pct_encoded, sub_delims, colon, commercial_at)
      .matched ("pchar", r);
  }
  // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
  static auto userinfo (rule r) {
    return r
      .star ([] (rule r2) {
        return r2.alternative (unreserved, pct_encoded, sub_delims, colon)
          .matched ("userinfo/*", r2);
      })
      .matched ("userinfo", r);
  }
  // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
  static auto scheme (rule r) {
    return r.concat (alpha)
      .star ([] (rule r2) {
        return r2.alternative (alpha, digit, plus, minus, full_stop)
          .matched ("scheme/*", r2);
      })
      .matched ("scheme", r);
  }
  // reg-name      = *( unreserved / pct-encoded / sub-delims )
  static auto reg_name (rule r) {
    return r
      .star ([] (rule r1) {
        return r1
          .alternative (uri::unreserved, uri::pct_encoded, uri::sub_delims)
          .matched ("reg-name/*", r1);
      })
      .matched ("reg-name", r);
  }
  // dec-octet     = DIGIT                 ; 0-9
  //               / %x31-39 DIGIT         ; 10-99
  //               / "1" 2DIGIT            ; 100-199
  //               / "2" %x30-34 DIGIT     ; 200-249
  //               / "25" %x30-35          ; 250-255
  static auto dec_octet (rule r) {
    return r
      .alternative (
        [] (rule r4) {
          return r4                          // 250-255
            .concat (single_char ('2'))      // "2"
            .concat (single_char ('5'))      // "5"
            .concat (char_range ('0', '5'))  // %x30-35
            .matched ("\"25\" %x30-35", r4);
        },
        [] (rule r3) {
          return r3                          // 200-249
            .concat (single_char ('2'))      // "2"
            .concat (char_range ('0', '4'))  // %x30-34
            .concat (digit)                  // DIGIT
            .matched ("\"2\" %x30-34 DIGIT", r3);
        },
        [] (rule r2) {
          return r2                      // 100-199
            .concat (single_char ('1'))  // "1"
            .concat (digit)              // 2DIGIT
            .concat (digit)              // (...)
            .matched ("\"1\" 2DIGIT", r2);
        },
        [] (rule r1) {
          return r1                          // 10-99
            .concat (char_range ('1', '9'))  // %x31-39
            .concat (digit)                  // DIGIT
            .matched ("%x31-39 DIGIT", r1);
        },
        digit)
      .matched ("dec-octet", r);
  }
  // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
  static auto ipv4address (rule r) {
    return r
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .matched ("IPv4address", r);
  }
  // h16 = 1*4HEXDIG
  static auto h16 (rule r) { return r.star (hexdig, 1, 4).matched ("h16", r); }
  // h16colon = h16 ":"
  static auto h16_colon (rule r) {
    return r.concat (h16).concat (single_colon).matched ("h16:", r);
  }
  static auto colon_colon (rule r) {
    return r.concat (colon).concat (colon).matched ("\"::\"", r);
  }
  // ls32          = ( h16 ":" h16 ) / IPv4address
  static auto ls32 (rule r) {
    return r
      .alternative (
        [] (rule r1) {
          return r1.concat (uri::h16).concat (colon).concat (uri::h16).matched (
            "h16:h16", r1);
        },
        uri::ipv4address)
      .matched ("ls32", r);
  }
  // IPv6address =                            6( h16 ":" ) ls32 // r1
  //             /                       "::" 5( h16 ":" ) ls32 // r2
  //             / [               h16 ] "::" 4( h16 ":" ) ls32 // r3
  //             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32 // r4
  //             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32 // r5
  //             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32 // r6
  //             / [ *4( h16 ":" ) h16 ] "::"              ls32 // r7
  //             / [ *5( h16 ":" ) h16 ] "::"              h16  // r8
  //             / [ *6( h16 ":" ) h16 ] "::"                   // r9
  static auto ipv6address (rule r) {
    return r
      .alternative (
        [] (rule r1) {
          // 6( h16 ":" ) ls32
          return r1.star (uri::h16_colon, 6, 6)
            .concat (uri::ls32)
            .matched ("6( h16: ) ls32", r1);
        },
        [] (rule r2) {
          // "::" 5( h16 ":" ) ls32
          return r2.concat (uri::colon_colon)
            .star (uri::h16_colon, 5, 5)
            .concat (uri::ls32)
            .matched ("\"::\" 5( h16 colon ) ls32", r2);
        },
        [] (rule r3) {
          // [ h16 ] "::" 4( h16 ":" ) ls32
          return r3.optional (uri::h16)
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 4, 4)
            .concat (uri::ls32)
            .matched ("[ h16 ] \"::\" 4( h16 colon ) ls32", r3);
        },
        [] (rule r4) {
          // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
          return r4
            .optional ([] (rule r4a) {
              return r4a.star (uri::h16_colon, 0, 1)
                .concat (uri::h16)
                .matched ("*1( h16 colon ) h16", r4a);
            })
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 3, 3)
            .concat (uri::ls32)
            .matched ("[ *1( h16 colon ) h16 ] \"::\" 3( h16 colon ) ls32", r4);
        },
        [] (rule r5) {
          // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
          return r5
            .optional ([] (rule r5a) {
              return r5a.star (uri::h16_colon, 0, 2)
                .concat (uri::h16)
                .matched ("*2( h16 colon ) h16", r5a);
            })
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 2, 2)
            .concat (uri::ls32)
            .matched ("[ *2( h16 colon ) h16 ] \"::\" 2( h16 colon ) ls32", r5);
        },
        [] (rule r6) {
          // [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
          return r6
            .optional ([] (rule r6a) {
              return r6a.star (uri::h16_colon, 0, 3)
                .concat (uri::h16)
                .matched ("*3( h16 colon ) h16", r6a);
            })
            .concat (uri::colon_colon)
            .concat (uri::h16_colon)
            .concat (uri::ls32)
            .matched ("[ *3( h16 colon ) h16 ] \"::\" h16 colon ls32", r6);
        },
        [] (rule r7) {
          // [ *4( h16 ":" ) h16 ] "::" ls32
          return r7
            .optional ([] (rule r7a) {
              return r7a.star (uri::h16_colon, 0, 4)
                .concat (uri::h16)
                .matched ("*4( h16 colon ) h16", r7a);
            })
            .concat (uri::colon_colon)
            .concat (uri::ls32)
            .matched ("[ *4( h16 colon ) h16 ] \"::\" ls32", r7);
        },
        [] (rule r8) {
          // [ *5( h16 ":" ) h16 ] "::" h16
          return r8
            .optional ([] (rule r8a) {
              return r8a.star (uri::h16_colon, 0, 5)
                .concat (uri::h16)
                .matched ("*5( h16 colon ) h16", r8a);
            })
            .concat (uri::colon_colon)
            .concat (uri::h16)
            .matched ("[ *5( h16 colon ) h16 ] \"::\" h16", r8);
        },
        [] (rule r9) {
          // [ *6( h16 ":" ) h16 ] "::"
          return r9
            .optional ([] (rule r9a) {
              return r9a.star (uri::h16_colon, 0, 6)
                .concat (uri::h16)
                .matched ("*6( h16 colon ) h16", r9a);
            })
            .concat (uri::colon_colon)
            .matched ("[ *6( h16 colon ) h16 ] \"::\"", r9);
        })
      .matched ("IPv6address", r);
  }
  // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
  static auto ipvfuture (rule r) {
    return r.concat (single_char ('v'))
      .star (hexdig, 1)
      .concat (full_stop)
      .star (
        [] (rule r1) {
          return r1.alternative (uri::unreserved, uri::sub_delims, colon)
            .matched ("unreserved / sub-delims / colon", r1);
        },
        1)
      .matched ("IPvFuture", r);
  }
  // IP-literal    = "[" ( IPv6address / IPvFuture ) "]"
  static auto ip_literal (rule r) {
    return r.concat (left_square_bracket)
      .concat ([] (rule r1) {
        return r1.alternative (uri::ipv6address, uri::ipvfuture)
          .matched ("IPv6address / IPvFuture", r1);
      })
      .concat (right_square_bracket)
      .matched ("IP-literal", r);
  }

  auto host_rule () {
    // host          = IP-literal / IPv4address / reg-name
    return [&result = result_] (rule r) {
      return r
        .concat (
          [] (rule r1) {
            return r1
              .alternative (uri::ip_literal, uri::ipv4address, uri::reg_name)
              .matched ("IP-literal / IPv4address / reg-name", r1);
          },
          [&result] (std::string_view host) { result.host = host; })
        .matched ("host", r);
    };
  }
  auto userinfo_at () {
    // userinfo-at = userinfo "@"
    return [&result = result_] (rule r) {
      return r
        .concat (uri::userinfo,
                 [&result] (std::string_view const userinfo) {
                   result.userinfo = userinfo;
                 })
        .concat (commercial_at)
        .matched ("userinfo \"@\"", r);
    };
  }

  // port = *DIGIT
  static auto port (rule r) { return r.star (digit).matched ("port", r); }

  auto colon_port () {
    // colon-port = ":" port
    return [&result = result_] (rule r) {
      return r.concat (colon)
        .concat (port,
                 [&result] (std::string_view const p) { result.port = p; })
        .matched ("\":\" port", r);
    };
  }

  auto authority () {
    // authority = [ userinfo "@" ] host [ ":" port ]
    return [this] (rule r) {
      return r.optional (this->userinfo_at ())
        .concat (this->host_rule ())
        .optional (this->colon_port ())
        .matched ("authority", r);
    };
  }

  // segment       = *pchar
  static auto segment (rule r) {
    return r.star (uri::pchar).matched ("segment", r);
  }
  // segment-nz    = 1*pchar
  static auto segment_nz (rule r) {
    return r.star (uri::pchar, 1U).matched ("segment-nz", r);
  }
  // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
  //                  ; non-zero-length segment without any colon ":"
  static auto segment_nz_nc (rule r) {
    return r
      .star (
        [] (rule r2) {
          return r2
            .alternative (uri::unreserved, uri::pct_encoded, uri::sub_delims,
                          commercial_at)
            .matched ("unreserved / pct-encoded / sub-delims / \"@\"", r2);
        },
        1U)
      .matched ("segment-nz-nc", r);
  }
  auto path_abempty () {
    // path-abempty  = *( "/" segment )
    return [&result = result_] (rule r) {
      return r
        .star ([&result] (rule r2) {
          return r2
            .concat (solidus,
                     [&result] (std::string_view const seg) {
                       result.segments.emplace_back (seg);
                     })
            .concat (segment,
                     [&result] (std::string_view const seg) {
                       result.segments.back () += seg;
                     })
            .matched ("\"/\" segment", r2);
        })
        .matched ("path-abempty", r);
    };
  }
  auto path_absolute () {
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    return [&result = result_] (rule r) {
      return r
        .concat (solidus,
                 [&result] (std::string_view const seg) {
                   result.segments.emplace_back (seg);
                 })
        .optional ([&result] (rule r1) {
          return r1
            .concat (uri::segment_nz,
                     [&result] (std::string_view seg) {
                       result.segments.back () += seg;
                     })
            .concat ([&result] (rule r2) {
              return r2
                .star ([&result] (rule r3) {
                  return r3
                    .concat (solidus,
                             [&result] (std::string_view const seg) {
                               result.segments.emplace_back (seg);
                             })
                    .concat (segment,
                             [&result] (std::string_view const seg) {
                               result.segments.back () += seg;
                             })
                    .matched ("\"/\" segment", r3);
                })
                .matched ("*( \"/\" segment )", r2);
            })
            .matched ("*( \"/\" segment )", r1);
        })
        .matched ("path-absolute", r);
    };
  }
  // path-empty    = 0<pchar>
  static auto path_empty (rule r) {
    return r.star (uri::pchar, 0, 0).matched ("path-empty", r);
  }
  // path-rootless = segment-nz *( "/" segment )
  auto path_rootless () {
    return [&result = result_] (rule r) {
      return r
        .concat (uri::segment_nz,
                 [&result] (std::string_view const seg) {
                   result.segments.emplace_back (seg);
                 })
        .star ([&result] (rule r1) {
          return r1
            .concat (solidus,
                     [&result] (std::string_view const seg) {
                       result.segments.emplace_back (seg);
                     })
            .concat (segment,
                     [&result] (std::string_view const seg) {
                       result.segments.back () += seg;
                     })
            .matched ("\"/\" segment", r1);
        })
        .matched ("path-rootless", r);
    };
  }

  // hier-part     = "//" authority path-abempty
  //               / path-absolute
  //               / path-rootless
  //               / path-empty
  auto hier_part () {
    return [this] (rule r) {
      return r
        .alternative (
          [this] (rule r1) {
            return r1.concat (solidus)
              .concat (solidus)
              .concat (this->authority ())
              .concat (this->path_abempty ())
              .matched ("\"//\" authority path-abempty", r1);
          },
          this->path_absolute (), this->path_rootless (), path_empty)
        .matched ("hier-part", r);
    };
  }

  // query         = *( pchar / "/" / "?" )
  static auto query (rule r) {
    return r
      .star ([] (rule r2) {
        return r2.alternative (uri::pchar, solidus, question_mark)
          .matched ("pchar / \"/\" / \"?\"", r2);
      })
      .matched ("query", r);
  }
  // fragment      = *( pchar / "/" / "?" )
  static auto fragment (rule r) { return query (r); }

public:
  // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
  std::optional<uri_parts> uri_rule (std::string_view const in) {
    bool success =
      rule{in}
        .concat (uri::scheme,
                 [&result = result_] (std::string_view const scheme) {
                   result.scheme = scheme;
                 })
        .concat (colon)
        .concat (this->hier_part ())
        .optional ([&result = result_] (rule rq) {
          // "?" query
          return rq
            .concat (question_mark)  // "?"
            .concat (uri::query,     // query
                     [&result] (std::string_view const query) {
                       result.query = query;
                     })
            .matched ("\"?\" query", rq);
        })
        .optional ([&result = result_] (rule rf) {
          // "#" fragment
          return rf
            .concat (hash)  // "#"
            .concat (       // fragment
              uri::fragment,
              [&result] (std::string_view const fragment) {
                result.fragment = fragment;
              })
            .matched ("\"#\" fragment", rf);
        })
        .done ();
    if (success) {
      return result_;
    }
    return {};
  }

  void clear () { result_.clear (); }
  uri_parts const& result () const { return result_; }

private:
  uri_parts result_;
};

void test_ab () {
  std::vector<std::string> output;
  bool ok =
    rule ("ab")
      .concat ([] (rule r) { return r.single_char ('a'); },
               [&output] (std::string_view str) { output.emplace_back (str); })
      .concat ([] (rule r) { return r.single_char ('b'); },
               [&output] (std::string_view str) { output.emplace_back (str); })
      .done ();
  assert (ok);
  assert (output.size () == 2);
  assert (output[0] == "a");
  assert (output[1] == "b");
}

void test_alternative () {
  std::vector<std::string> output;
  auto remember = [&output] (std::string_view str) {
    output.emplace_back (str);
  };
  auto b = [&remember] (rule r) {
    return r.concat (single_char ('b'), remember).matched ("b", r);
  };
  auto c = [&remember] (rule r) {
    return r.concat (single_char ('c'), remember).matched ("c", r);
  };
  {
    output.clear ();
    bool ok = rule ("ac")
                .concat (single_char ('a'), remember)
                .alternative (b, c)
                .done ();
    assert (ok);
    assert (output.size () == 2);
    assert (output[0] == "a");
    assert (output[1] == "c");
  }
  {
    output.clear ();
    bool ok = rule ("ab")
                .concat (single_char ('a'), remember)
                .alternative (b, c)
                .done ();
    assert (ok);
    assert (output.size () == 2);
    assert (output[0] == "a");
    assert (output[1] == "b");
  }
  {
    output.clear ();
    bool ok = rule ("ad")
                .concat (single_char ('a'), remember)
                .alternative (b, c)
                .done ();
    assert (!ok);
    assert (output.empty ());
  }
}

void test_star () {
  std::vector<std::string> output;
  auto remember = [&output] (std::string_view str) {
    output.emplace_back (str);
  };

  auto a = [&remember] (rule r) {
    return r.concat (single_char ('a'), remember).matched ("a", r);
  };
  output.clear ();
  bool ok = rule ("aaa").star (a).done ();
  assert (ok);
  assert (output.size () == 3);
  assert (output[0] == "a");
  assert (output[1] == "a");
  assert (output[2] == "a");
}

void test_star2 () {
  std::vector<std::string> output;
  int ctr = 0;
  auto remember = [&output] (std::string_view str) {
    output.emplace_back (str);
  };

  bool ok =
    rule ("/")
      .star ([&remember] (rule r1) {
        return r1.concat (single_char ('/'), remember)
          .concat (
            [] (rule r2) {
              return r2.star (char_range ('a', 'z')).matched ("a-z", r2);
            },
            remember)
          .matched ("*(a-z)", r1);
      })
      .done ();
  assert (output.size () == 2);
  assert (output[0] == "/");
  assert (output[1] == "");
}

void test () {
  test_ab ();
  test_alternative ();
  test_star ();
  test_star2 ();
}

void read_stream (std::istream& is) {
  std::string line;
  while (getline (is, line)) {
    std::cout << "URI: " << line << '\n';

    uri x;
    auto r = x.uri_rule (line);
    if (!r) {
      std::exit (EXIT_FAILURE);
    }
    auto value_or_none = [] (std::optional<std::string> const s) {
      return s ? "\""s + s.value () + "\""s : "None"s;
    };
    std::cout << " scheme: " << value_or_none (r->scheme) << '\n'
              << " userinfo: " << value_or_none (r->userinfo) << '\n'
              << " host: " << value_or_none (r->host) << '\n'
              << " port: " << value_or_none (r->port) << '\n';
    std::string path;
    for (auto const& s : r->segments) {
      path += s;
    }
    std::cout << " path: " << std::quoted (path) << '\n';
    std::cout << " query: " << value_or_none (r->query) << '\n';
    std::cout << " fragment: " << value_or_none (r->fragment) << '\n';
  }
}

int main (int argc, char const* argv[]) {
  test ();
  if (argc == 1) {
    // read_stream (std::cin);
    std::istringstream in{"Z://-@[b8::C:AB:2b]:16?%FC:"};
    read_stream (in);
  } else {
    for (int arg = 1; arg < argc; ++arg) {
      std::filesystem::path const p = argv[arg];
      std::ifstream infile{p};
      if (!infile.is_open ()) {
        std::cerr << "Error: couldn't open " << p << '\n';
        std::exit (EXIT_FAILURE);
      }
      read_stream (infile);
    }
  }
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
