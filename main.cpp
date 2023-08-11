#include <array>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <iterator>
#include <optional>

using namespace std::literals;

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

  template <typename MatchFunction, typename ActionFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, std::string_view> && std::is_invocable_v<ActionFunction, std::string_view>>>
  rule concat (MatchFunction match, ActionFunction action) {
    return concat_impl (match, action, false);
  }
  template <typename MatchFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, std::string_view>>>
  rule concat (MatchFunction match) {
    return concat(match, [] (std::string_view /*str*/) {/*nop*/});
  }

  template <typename MatchFunction, typename ActionFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, std::string_view> && std::is_invocable_v<ActionFunction, std::string_view>>>
  rule concat_opt (MatchFunction match, ActionFunction action) {
    return concat_impl (match, action, true);
  }
  template <typename MatchFunction>
  rule concat_opt (MatchFunction match) {
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
  template <typename MatchFunction, typename = std::enable_if_t<std::is_invocable_v<MatchFunction, std::string_view>>>
  rule star (MatchFunction match, unsigned min = 0, unsigned max = std::numeric_limits<unsigned>::max());



  rule alternative () { return rule{}; }
  template <typename MatchFunction, typename ...Rest>
  rule alternative (MatchFunction match, Rest && ...rest) {
    if (!tail_) {
      // If matching has already failed, then pass that condition down the chain.
      return *this;
    }
    if (auto const m = match (*tail_)) {
      return rule{tail_->substr (m->length())};
    }
    // This didn't match, so try the next one.
    return this->alternative(std::forward<Rest> (rest)...);
  }


//  constexpr std::optional<std::string_view> tail () const { return tail_; }

  std::optional<std::string_view> head (std::string_view const in) {
    if (tail_) {
      return in.substr(0, in.length() - tail_->length());
    }
    return {};
  }

private:
  template <typename MatchFunction, typename ActionFunction>
  rule concat_impl (MatchFunction match, ActionFunction action, bool optional);

  std::optional<std::string_view> tail_;
};

#if 0
std::optional<std::string_view> alternative (std::string_view /*in*/) { return {}; }
template <typename MatchFunction, typename ...Rest>
std::optional<std::string_view> alternative (std::string_view in, MatchFunction match, Rest && ...rest) {
  if (auto const m = match (in)) {
    return m;
  }
  return alternative(in, std::forward<Rest> (rest)...);
}
#endif


template <typename MatchFunction, typename>
rule rule::star (MatchFunction match, unsigned min, unsigned max) {
  if (!tail_) {
    return *this;
  }
  auto length = std::string_view::size_type{0};
  std::string_view str = *tail_;
  auto done = false;
  auto count = 0U;
  while (!done) {
    auto const m = match (str);
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

template <typename MatchFunction, typename ActionFunction>
rule rule::concat_impl (MatchFunction match, ActionFunction action, bool optional) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (auto const m = match (*tail_)) {
    action (*m);
    return rule{tail_->substr (m->length())};
  }
  return optional ? rule{tail_} : rule{}; // Matching failed.
}




// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
constexpr bool is_unreserved (char c) noexcept {
  return std::isalnum (c) || c == '-' || c == '.' || c == '_' || c == '~';
}
std::optional<std::string_view> unreserved (std::string_view const in) {
  if (!in.empty() && is_unreserved(in.front())) {
    return in.substr(0, 1);
  }
  return {};
}
// gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
constexpr bool is_gen_delim (char const c) noexcept {
  return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@';
}
// sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//               / "*" / "+" / "," / ";" / "="
constexpr bool is_sub_delim (char const c) noexcept {
  return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c ==  '*' || c ==  '+' || c == ',' || c == ';' || c == '=';
}
std::optional<std::string_view> sub_delims (std::string_view const in) {
  if (!in.empty() && is_sub_delim(in.front())) {
    return in.substr(0, 1);
  }
  return {};
}

// reserved      = gen-delims / sub-delims
constexpr bool is_reserved (char const c) {
  return is_gen_delim(c) || is_sub_delim(c);
}
// pct-encoded   = "%" HEXDIG HEXDIG
std::optional<std::string_view> pct_encoded (std::string_view const in) {
  if (in.length () >= 3 && in[0] == '%' && std::isxdigit(in[1]) && std::isxdigit(in[2])) {
    return in.substr (0, 3);
  }
  return {};
}

rule single_char_rule (rule in, char const c) {
  return in.concat([c] (std::string_view const s) -> std::optional<std::string_view> {
    if (!s.empty () && s.front() == c) {
      return s.substr(0,1);
    }
    return {};
  });
}

decltype(auto) commercial_at (std::string_view const in) {
  return single_char_rule (rule{in}, '@').head(in);
}
decltype(auto) colon (std::string_view const in) {
  return single_char_rule (rule{in}, ':').head(in);
}
decltype(auto) hash (std::string_view const in) {
  return single_char_rule (rule{in}, '#').head(in);
}
decltype(auto) solidus (std::string_view const in) {
  return single_char_rule (rule{in}, '/').head(in);
}
decltype(auto) question_mark (std::string_view const in) {
  return single_char_rule (rule{in}, '?').head(in);
}

// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
std::optional<std::string_view> pchar (std::string_view const in) {
  return rule{in}.alternative (unreserved, pct_encoded, sub_delims, colon, commercial_at).head(in);
}

//authority     = [ userinfo "@" ] host [ ":" port ]
//userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
std::optional<std::string_view> userinfo_rule (std::string_view const in) {
  return rule{in}.star ([] (std::string_view const str) {
    return rule{str}.alternative (unreserved, pct_encoded, sub_delims, colon).head(str);
  }).head(in);
}



// scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
std::optional<std::string_view> scheme (std::string_view const in) {
  if (in.empty () || !std::isalpha(in.front())) {
    return {};
  }
  auto pos = std::string_view::size_type{1};
  auto const length = in.length ();
  auto const is_part = [] (char c) { return std::isalnum(c) || c == '+' || c == '-' || c == '.'; };
  for (; pos < length && is_part (in[pos]); ++pos) {
  }
  return in.substr (0, pos);
}

// reg-name      = *( unreserved / pct-encoded / sub-delims )
std::optional<std::string_view> reg_name_rule (std::string_view const in) {
  return rule{in}.star ([] (std::string_view const str) {
    return rule{str}.alternative (unreserved, pct_encoded, sub_delims).head(str);
  }).head(in);
}



// host          = IP-literal / IPv4address / reg-name
std::optional<std::string_view> host_rule (std::string_view const in) {
  auto r = reg_name_rule (in);
  if (r) {
    auto host = in.substr(0,r->length());
std::cout << "host: " << std::quoted(host) << '\n';
  }
  return r;
}

// userinfo-at = userinfo "@"
rule userinfo_at (rule in) {
  return in
    .concat (userinfo_rule, [] (std::string_view const userinfo) {
std::cout << "userinfo: " << std::quoted(userinfo) << '\n';
    })
    .concat(commercial_at);
}

rule digit (rule in) {
  return in.concat([] (std::string_view const s) -> std::optional<std::string_view> {
    if (!s.empty() && std::isdigit(s.front())) {
      return s.substr(0,1);
    }
    return {};
  });
}

// port          = *DIGIT
std::optional<std::string_view> port (std::string_view const in) {
  return rule{in}.star([] (std::string_view s) {
    return digit(rule{s}).head(s);
  }).head(in);
}

// colon-port = ":" port
std::optional<std::string_view> colon_port (std::string_view const in) {
  return rule{in}.concat (colon).concat(port, [] (std::string_view const p) {
std::cout << "port: " << std::quoted(p) << '\n';
  }).head(in);
}

//authority     = [ userinfo "@" ] host [ ":" port ]
rule authority (rule in) {
  return in
    .concat_opt([] (std::string_view str) { return userinfo_at (rule{str}).head (str); })
    .concat (host_rule)
    .concat_opt (colon_port);
}


//segment       = *pchar
std::optional<std::string_view> segment (std::string_view const in) {
  return rule{in}.star (pchar).head(in);
}

// segment-nz    = 1*pchar
std::optional<std::string_view> segment_nz (std::string_view const in) {
  return rule{in}.star (pchar, 1U).head(in);
}

// segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
//                  ; non-zero-length segment without any colon ":"
std::optional<std::string_view> segment_nz_nc (std::string_view const in) {
  return rule{in}.star ([] (std::string_view const str) {
    return rule{str}.alternative(unreserved, pct_encoded, sub_delims, commercial_at).head(str);
  }, 1U).head(in);
}

// path-abempty  = *( "/" segment )
std::optional<std::string_view> path_abempty (std::string_view const in) {
  return rule{in}.star ([] (std::string_view str) {
    return rule{str}.concat(solidus).concat(segment, [] (std::string_view const seg) {
std::cout << "segment: " << std::quoted(seg) << '\n';
    }).head(str);
  }).head(in);
}

//path-absolute = "/" [ segment-nz *( "/" segment ) ]
std::optional<std::string_view> path_absolute (std::string_view const in) {
  return rule{in}
    .concat (solidus)
    .concat_opt ([] (std::string_view const str) {
      return rule{str}
        .concat (segment_nz)
        .concat ([] (std::string_view const s2) {
          return rule{s2}.star ([] (std::string_view const s3) {
            return rule{s3}.concat (solidus).concat(segment).head(s3);
          }).head(s2);
        }).head(str);
    }).head(in);
}

//path-noscheme = segment-nz-nc *( "/" segment )
//path-rootless = segment-nz *( "/" segment )
//path-empty    = 0<pchar>



// hp1     = "//" authority path-abempty
std::optional<std::string_view> hp1 (std::string_view const in) {
  return rule{in}
    .concat(solidus)
    .concat(solidus)
    .concat([] (std::string_view str) { return authority(rule{str}).head(str); })
    .concat(path_abempty)
    .head(in);
}

// hier-part     = hp1 / path-absolute / path-rootless / path-empty
rule hier_part_rule (rule in) {
  // TODO: path-rootless
  // TODO: path-empty
  return in.alternative(hp1, path_absolute);
}

// query         = *( pchar / "/" / "?" )
std::optional<std::string_view> query (std::string_view const in) {
  return rule{in}.star ([] (std::string_view const str) {
    return rule{str}.alternative (pchar, solidus, question_mark).head(str);
  }).head(in);
}

rule question_query (rule in) {
  return in
    .concat (question_mark)
    .concat (query, [] (std::string_view const query) {
std::cout << "query: " << std::quoted(query) << '\n';
    });
}

// fragment      = *( pchar / "/" / "?" )
std::optional<std::string_view> fragment (std::string_view const in) {
  return query (in);
}

rule hash_fragment (rule in) {
  return in
    .concat (hash)
    .concat (fragment, [] (std::string_view const fragment) {
      std::cout << "fragment: " << std::quoted(fragment) << '\n';
    });
}

// URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
std::optional<std::string_view> uri_rule (std::string_view const in) {
  return rule{in}
    .concat (scheme, [] (std::string_view const scheme) {
      std::cout << "scheme: " << std::quoted (scheme) << '\n';
    })
    .concat (colon)
    .concat ([] (std::string_view const in) { return hier_part_rule (rule{in}).head (in); })
    .concat_opt ([] (std::string_view const in) { return question_query (rule{in}).head (in); })
    .concat_opt ([] (std::string_view const in) { return hash_fragment (rule{in}).head (in); })
    .head(in);
  return {};
}

int main() {

//host          = IP-literal / IPv4address / reg-name
//authority     = [ userinfo "@" ] host [ ":" port ]
//userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )

  // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
  // hier-part     = "//" authority path-abempty
  //               / path-absolute
  //               / path-rootless
  //               / path-empty
  auto const uri = "https://userinfo@foo.com:443/ab/./cd/../foo%20bar?query#fragment"sv;
  //auto const uri = "https://foo.com//ab/./cd/../foo%20bar"sv;
std::cout << "URI: " << uri << '\n';
  uri_rule (uri);

  std::vector<std::string> out = remove_dot_segments (uri);
  std::string result;
  for (auto const & s: out) {
    result += '/';
    result += s;
  }
  std::cout << std::quoted(result) << '\n';

}
