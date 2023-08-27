#include "uri.hpp"

#include <sstream>

#include "rule.hpp"

namespace {

auto single_colon (rule const& r) {
  return r.concat (colon)
    .concat ([] (rule const& r1) -> rule::matched_result {
      if (auto const& sv = r1.tail ()) {
        if (sv->empty () || sv->front () != ':') {
          return std::make_tuple (sv->substr (0, 0),
                                  rule::acceptor_container{});
        }
      }
      return {};
    })
    .matched ("single-colon", r);
}

// sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//               / "*" / "+" / "," / ";" / "="
auto sub_delims (rule const& r) {
  return r.single_char ([] (char const c) {
    return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
           c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
  });
}

// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
auto unreserved (rule const& r) {
  return r.single_char ([] (char const c) {
    return static_cast<bool> (std::isalnum (static_cast<int> (c))) ||
           c == '-' || c == '.' || c == '_' || c == '~';
  });
}

// pct-encoded   = "%" HEXDIG HEXDIG
auto pct_encoded (rule const& r) {
  return r.concat (single_char ('%'))
    .concat (hexdig)
    .concat (hexdig)
    .matched ("pct-encoded", r);
}

// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
auto pchar (rule const& r) {
  return r
    .alternative (unreserved, pct_encoded, sub_delims, colon, commercial_at)
    .matched ("pchar", r);
}

// userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
auto userinfo (rule const& r) {
  return r
    .star ([] (rule const& r2) {
      return r2.alternative (unreserved, pct_encoded, sub_delims, colon)
        .matched ("userinfo/*", r2);
    })
    .matched ("userinfo", r);
}

// scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
auto scheme (rule const& r) {
  return r.concat (alpha)
    .star ([] (rule const& r2) {
      return r2.alternative (alpha, digit, plus, minus, full_stop)
        .matched ("scheme/*", r2);
    })
    .matched ("scheme", r);
}

// reg-name      = *( unreserved / pct-encoded / sub-delims )
auto reg_name (rule const& r) {
  return r
    .star ([] (rule const& r1) {
      return r1.alternative (unreserved, pct_encoded, sub_delims)
        .matched ("reg-name/*", r1);
    })
    .matched ("reg-name", r);
}

// dec-octet     = DIGIT                 ; 0-9
//               / %x31-39 DIGIT         ; 10-99
//               / "1" 2DIGIT            ; 100-199
//               / "2" %x30-34 DIGIT     ; 200-249
//               / "25" %x30-35          ; 250-255
auto dec_octet (rule const& r) {
  return r
    .alternative (
      [] (rule const& r4) {
        return r4                          // 250-255
          .concat (single_char ('2'))      // "2"
          .concat (single_char ('5'))      // "5"
          .concat (char_range ('0', '5'))  // %x30-35
          .matched ("\"25\" %x30-35", r4);
      },
      [] (rule const& r3) {
        return r3                          // 200-249
          .concat (single_char ('2'))      // "2"
          .concat (char_range ('0', '4'))  // %x30-34
          .concat (digit)                  // DIGIT
          .matched ("\"2\" %x30-34 DIGIT", r3);
      },
      [] (rule const& r2) {
        return r2                      // 100-199
          .concat (single_char ('1'))  // "1"
          .concat (digit)              // 2DIGIT
          .concat (digit)              // (...)
          .matched ("\"1\" 2DIGIT", r2);
      },
      [] (rule const& r1) {
        return r1                          // 10-99
          .concat (char_range ('1', '9'))  // %x31-39
          .concat (digit)                  // DIGIT
          .matched ("%x31-39 DIGIT", r1);
      },
      digit)
    .matched ("dec-octet", r);
}

// IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
auto ipv4address (rule const& r) {
  return r
    .concat (dec_octet)  // dec-octet
    .concat (full_stop)  // "."
    .concat (dec_octet)  // dec-octet
    .concat (full_stop)  // "."
    .concat (dec_octet)  // dec-octet
    .concat (full_stop)  // "."
    .concat (dec_octet)  // dec-octet
    .matched ("IPv4address", r);
}

// h16 = 1*4HEXDIG
auto h16 (rule const& r) {
  return r.star (hexdig, 1, 4).matched ("h16", r);
}

// h16colon = h16 ":"
auto h16_colon (rule const& r) {
  return r.concat (h16).concat (single_colon).matched ("h16:", r);
}
auto colon_colon (rule const& r) {
  return r.concat (colon).concat (colon).matched ("\"::\"", r);
}

// ls32          = ( h16 ":" h16 ) / IPv4address
auto ls32 (rule const& r) {
  return r
    .alternative (
      [] (rule const& r1) {
        return r1.concat (h16).concat (colon).concat (h16).matched ("h16:h16",
                                                                    r1);
      },
      ipv4address)
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
auto ipv6address (rule const& r) {
  return r
    .alternative (
      [] (rule const& r1) {
        // 6( h16 ":" ) ls32
        return r1.star (h16_colon, 6, 6)
          .concat (ls32)
          .matched ("6( h16: ) ls32", r1);
      },
      [] (rule const& r2) {
        // "::" 5( h16 ":" ) ls32
        return r2.concat (colon_colon)
          .star (h16_colon, 5, 5)
          .concat (ls32)
          .matched ("\"::\" 5( h16 colon ) ls32", r2);
      },
      [] (rule const& r3) {
        // [ h16 ] "::" 4( h16 ":" ) ls32
        return r3.optional (h16)
          .concat (colon_colon)
          .star (h16_colon, 4, 4)
          .concat (ls32)
          .matched ("[ h16 ] \"::\" 4( h16 colon ) ls32", r3);
      },
      [] (rule const& r4) {
        // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
        return r4
          .optional ([] (rule const& r4a) {
            return r4a.star (h16_colon, 0, 1)
              .concat (h16)
              .matched ("*1( h16 colon ) h16", r4a);
          })
          .concat (colon_colon)
          .star (h16_colon, 3, 3)
          .concat (ls32)
          .matched ("[ *1( h16 colon ) h16 ] \"::\" 3( h16 colon ) ls32", r4);
      },
      [] (rule const& r5) {
        // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
        return r5
          .optional ([] (rule const& r5a) {
            return r5a.star (h16_colon, 0, 2)
              .concat (h16)
              .matched ("*2( h16 colon ) h16", r5a);
          })
          .concat (colon_colon)
          .star (h16_colon, 2, 2)
          .concat (ls32)
          .matched ("[ *2( h16 colon ) h16 ] \"::\" 2( h16 colon ) ls32", r5);
      },
      [] (rule const& r6) {
        // [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
        return r6
          .optional ([] (rule const& r6a) {
            return r6a.star (h16_colon, 0, 3)
              .concat (h16)
              .matched ("*3( h16 colon ) h16", r6a);
          })
          .concat (colon_colon)
          .concat (h16_colon)
          .concat (ls32)
          .matched ("[ *3( h16 colon ) h16 ] \"::\" h16 colon ls32", r6);
      },
      [] (rule const& r7) {
        // [ *4( h16 ":" ) h16 ] "::" ls32
        return r7
          .optional ([] (rule const& r7a) {
            return r7a.star (h16_colon, 0, 4)
              .concat (h16)
              .matched ("*4( h16 colon ) h16", r7a);
          })
          .concat (colon_colon)
          .concat (ls32)
          .matched ("[ *4( h16 colon ) h16 ] \"::\" ls32", r7);
      },
      [] (rule const& r8) {
        // [ *5( h16 ":" ) h16 ] "::" h16
        return r8
          .optional ([] (rule const& r8a) {
            return r8a.star (h16_colon, 0, 5)
              .concat (h16)
              .matched ("*5( h16 colon ) h16", r8a);
          })
          .concat (colon_colon)
          .concat (h16)
          .matched ("[ *5( h16 colon ) h16 ] \"::\" h16", r8);
      },
      [] (rule const& r9) {
        // [ *6( h16 ":" ) h16 ] "::"
        return r9
          .optional ([] (rule const& r9a) {
            return r9a.star (h16_colon, 0, 6)
              .concat (h16)
              .matched ("*6( h16 colon ) h16", r9a);
          })
          .concat (colon_colon)
          .matched ("[ *6( h16 colon ) h16 ] \"::\"", r9);
      })
    .matched ("IPv6address", r);
}

// IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
auto ipvfuture (rule const& r) {
  return r.concat (single_char ('v'))
    .star (hexdig, 1)
    .concat (full_stop)
    .star (
      [] (rule const& r1) {
        return r1.alternative (unreserved, sub_delims, colon)
          .matched ("unreserved / sub-delims / colon", r1);
      },
      1)
    .matched ("IPvFuture", r);
}

// IP-literal    = "[" ( IPv6address / IPvFuture ) "]"
auto ip_literal (rule const& r) {
  return r.concat (left_square_bracket)
    .concat ([] (rule const& r1) {
      return r1.alternative (ipv6address, ipvfuture)
        .matched ("IPv6address / IPvFuture", r1);
    })
    .concat (right_square_bracket)
    .matched ("IP-literal", r);
}

auto host_rule (uri::parts& result) {
  // host          = IP-literal / IPv4address / reg-name
  return [&result] (rule const& r) {
    return r
      .concat (
        [] (rule const& r1) {
          return r1.alternative (ip_literal, ipv4address, reg_name)
            .matched ("IP-literal / IPv4address / reg-name", r1);
        },
        [&result] (std::string_view host) { result.authority.host = host; })
      .matched ("host", r);
  };
}

auto userinfo_at (uri::parts& result) {
  // userinfo-at = userinfo "@"
  return [&result] (rule const& r) {
    return r
      .concat (userinfo,
               [&result] (std::string_view const userinfo) {
                 result.authority.userinfo = userinfo;
               })
      .concat (commercial_at)
      .matched ("userinfo \"@\"", r);
  };
}

// port = *DIGIT
auto port (rule const& r) {
  return r.star (digit).matched ("port", r);
}

auto colon_port (uri::parts& result) {
  // colon-port = ":" port
  return [&result] (rule const& r) {
    return r.concat (colon)
      .concat (port, [&result] (
                       std::string_view const p) { result.authority.port = p; })
      .matched ("\":\" port", r);
  };
}

auto authority (uri::parts& result) {
  // authority = [ userinfo "@" ] host [ ":" port ]
  return [&result] (rule const& r) {
    return r.optional (userinfo_at (result))
      .concat (host_rule (result))
      .optional (colon_port (result))
      .matched ("authority", r);
  };
}

// segment       = *pchar
auto segment (rule const& r) {
  return r.star (pchar).matched ("segment", r);
}

// segment-nz    = 1*pchar
auto segment_nz (rule const& r) {
  return r.star (pchar, 1U).matched ("segment-nz", r);
}

// segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
//                  ; non-zero-length segment without any colon ":"
auto segment_nz_nc (rule const& r) {
  return r
    .star (
      [] (rule const& r2) {
        return r2
          .alternative (unreserved, pct_encoded, sub_delims, commercial_at)
          .matched ("unreserved / pct-encoded / sub-delims / \"@\"", r2);
      },
      1U)
    .matched ("segment-nz-nc", r);
}

class record_path_abs {
public:
  explicit constexpr record_path_abs (uri::parts& result) noexcept
      : result_{result} {}
  void operator() (std::string_view const str) {
    (void)str;
    result_.path.absolute = true;
    result_.path.directory = true;
  }

private:
  uri::parts& result_;
};

class append_segment {
public:
  explicit constexpr append_segment (uri::parts& result) noexcept
      : result_{result} {}
  void operator() (std::string_view const seg) {
    if (seg.length () == 0) {
      result_.path.directory = true;
    } else {
      result_.path.directory = false;
      result_.path.segments.emplace_back (seg);
    }
  }

private:
  uri::parts& result_;
};

auto path_abempty (uri::parts& result) {
  // path-abempty  = *( "/" segment )
  return [&result] (rule const& r) {
    return r
      .star ([&result] (rule const& r2) {
        return r2.concat (solidus, record_path_abs{result})
          .concat (segment, append_segment{result})
          .matched ("\"/\" segment", r2);
      })
      .matched ("path-abempty", r);
  };
}

auto path_absolute (uri::parts& result) {
  // path-absolute = "/" [ segment-nz *( "/" segment ) ]
  return [&result] (rule const& r) {
    return r.concat (solidus, record_path_abs{result})
      .optional ([&result] (rule const& r1) {
        return r1.concat (segment_nz, append_segment{result})
          .concat ([&result] (rule const& r2) {
            return r2
              .star ([&result] (rule const& r3) {
                return r3.concat (solidus)
                  .concat (segment, append_segment{result})
                  .matched ("\"/\" segment", r3);
              })
              .matched ("*( \"/\" segment )", r2);
          })
          .matched ("*( \"/\" segment )", r1);
      })
      .matched ("path-absolute", r);
  };
}

// path-noscheme = segment-nz-nc *( "/" segment )
auto path_noscheme (uri::parts& result) {
  return [&result] (rule const& r) {
    return r.concat (segment_nz_nc, append_segment{result})
      .star ([&result] (rule const& r1) {
        return r1.concat (solidus)
          .concat (segment, append_segment{result})
          .matched ("\"/\" segment", r1);
      })
      .matched ("path-noscheme", r);
  };
}

// path-empty    = 0<pchar>
auto path_empty (rule const& r) {
  return r.star (pchar, 0, 0).matched ("path-empty", r);
}

// path-rootless = segment-nz *( "/" segment )
auto path_rootless (uri::parts& result) {
  return [&result] (rule const& r) {
    return r.concat (segment_nz, append_segment{result})
      .star ([&result] (rule const& r1) {
        return r1.concat (solidus)
          .concat (segment, append_segment{result})
          .matched ("\"/\" segment", r1);
      })
      .matched ("path-rootless", r);
  };
}

// relative-part = "//" authority path-abempty
//               / path-absolute
//               / path-noscheme
//               / path-empty
auto relative_part (uri::parts& result) {
  return [&result] (rule const& r) {
    return r
      .alternative (
        [&result] (rule const& r1) {
          return r1.concat (solidus)
            .concat (solidus)
            .concat (authority (result))
            .concat (path_abempty (result))
            .matched (R"("//" authority path-abempty)", r1);
        },
        path_absolute (result), path_noscheme (result), path_empty)
      .matched ("relative-part", r);
  };
}

// query         = *( pchar / "/" / "?" )
auto query (rule const& r) {
  return r
    .star ([] (rule const& r2) {
      return r2.alternative (pchar, solidus, question_mark)
        .matched (R"(pchar / "/" / "?")", r2);
    })
    .matched ("query", r);
}
// question-query = "?" query
auto question_query (uri::parts& result) {
  return [&result] (rule const& r) {
    return r
      .concat (question_mark)  // "?"
      .concat (query, [&result] (
                        std::string_view const query) { result.query = query; })
      .matched ("question-query", r);
  };
}

// fragment      = *( pchar / "/" / "?" )
auto fragment (rule const& r) {
  return query (r);
}

// hash-fragment = "#" fragment
auto hash_fragment (uri::parts& result) {
  return [&result] (rule const& rf) {
    return rf.concat (hash)
      .concat (fragment,
               [&result] (std::string_view const fragment) {
                 result.fragment = fragment;
               })
      .matched ("hash-fragment", rf);
  };
};

// relative-ref  = relative-part [ question-query ] [ hash-fragment ]
auto relative_ref (uri::parts& result) {
  return [&result] (rule const& r) {
    return r.concat (relative_part (result))
      .optional (question_query (result))
      .optional ([&result] (rule const& rf) {
        // "#" fragment
        return rf
          .concat (hash)  // "#"
          .concat (       // fragment
            fragment,
            [&result] (std::string_view const fragment) {
              result.fragment = fragment;
            })
          .matched ("\"#\" fragment", rf);
      })
      .matched ("relative-ref", r);
  };
}

// hier-part     = "//" authority path-abempty
//               / path-absolute
//               / path-rootless
//               / path-empty
auto hier_part (uri::parts& result) {
  return [&result] (rule const& r) {
    return r
      .alternative (
        [&result] (rule const& r1) {
          return r1.concat (solidus)
            .concat (solidus)
            .concat (authority (result))
            .concat (path_abempty (result))
            .matched ("\"//\" authority path-abempty", r1);
        },
        path_absolute (result), path_rootless (result), path_empty)
      .matched ("hier-part", r);
  };
}

// URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
auto URI (uri::parts& result) {
  return [&result] (rule const& r) {
    return r
      .concat (
        scheme,
        [&result] (std::string_view const scheme) { result.scheme = scheme; })
      .concat (colon)
      .concat (hier_part (result))
      .optional (question_query (result))
      .optional (hash_fragment (result))
      .matched ("URI", r);
  };
}

// URI-reference = URI / relative-ref
auto URI_reference (uri::parts& result) {
  return [&result] (rule const& r) {
    return r.alternative (URI (result), relative_ref (result))
      .matched ("URI-reference", r);
  };
}

#if 0
// absolute-URI  = scheme ":" hier-part [ "?" query ]
auto absolute_URI (uri::parts& result) {
  return [&result] (rule const& r) {
    return r
      .concat (
        scheme,
        [&result] (std::string_view const scheme) { result.scheme = scheme; })
      .concat (colon)
      .concat (hier_part (result))
      .optional (question_query (result))
      .matched ("absolute-URI", r);
  };
}
#endif

void lowercase (std::string& s) {
  std::transform (std::begin (s), std::end (s), std::begin (s), [] (char c) {
    return static_cast<char> (std::tolower (static_cast<int> (c)));
  });
}

// 5.2.3.  Merge Paths
//
// The pseudocode above refers to a "merge" routine for merging a
// relative-path reference with the path of the base URI.  This is
// accomplished as follows:
//
// o  If the base URI has a defined authority component and an empty
//    path, then return a string consisting of "/" concatenated with the
//    reference's path; otherwise,
//
// o  return a string consisting of the reference's path component
//    appended to all but the last segment of the base URI's path (i.e.,
//    excluding any characters after the right-most "/" in the base URI
//    path, or excluding the entire base URI path if it does not contain
//    any "/" characters).

uri::path_description merge (uri::parts const& Base, uri::parts const& R) {
  uri::path_description result;
  if (Base.authority && Base.path.empty ()) {
    result.absolute = true;
    result.directory = R.path.directory;
    result.segments = R.path.segments;
    return result;
  }

  auto last = std::end (Base.path.segments);
  if (Base.path.segments.size () > 1 && !Base.path.directory) {
    std::advance (last, -1);
  }

  result.absolute = Base.path.absolute;
  result.directory = R.path.directory;

  auto out = std::back_inserter (result.segments);
  out = std::copy (std::begin (Base.path.segments), last, out);
  std::copy (std::begin (R.path.segments), std::end (R.path.segments), out);
  return result;
}

}  // end anonymous namespace

namespace uri {

// Remove dot segments from the string.
//
// See also Section 5.2.4 of RFC 3986.
// http://tools.ietf.org/html/rfc3986#section-5.2.4
void path_description::remove_dot_segments () {
  path_container output;
  std::for_each (std::begin (segments), std::end (segments),
                 [&output] (std::string const& s) {
                   if (s == ".") {
                     // '.' means "the current directory": ignore it.
                     return;
                   }
                   if (s == "..") {
                     // ".." is "up one directory" so, if we can, pop the last
                     // element.
                     if (!output.empty ()) {
                       output.pop_back ();
                     }
                     return;
                   }
                   // Anything else just gets appended to the output.
                   output.push_back (s);
                 });
  segments = std::move (output);
}

path_description::operator std::filesystem::path () const {
  std::filesystem::path p;
  if (absolute) {
    p /= "/";
  }
  for (auto const& seg : segments) {
    p /= seg;
  }
  if (directory) {
    p /= "";
  }
  return p;
}

// URI-reference = URI / relative-ref
std::optional<parts> split (std::string_view const in) {
  parts result;
  if (rule{in}.alternative (URI (result), URI_reference (result)).done ()) {
    if (!result.path.segments.empty ()) {
      auto const& b = result.path.segments.back ();
      if (b == "." || b == "..") {
        result.path.directory = true;
      }
    }
    return result;
  }
  return {};
}

std::string percent_decode (std::string_view src) {
  std::string result;
  result.reserve (src.length ());
  for (auto pos = src.begin (), end = src.end (); pos != end; ++pos) {
    if (*pos == '%' && std::distance (pos, end) >= 3 &&
        std::isxdigit (*(pos + 1)) && std::isxdigit (*(pos + 2))) {
      auto hex2dec = [] (char const digit) {
        if (digit >= 'a' && digit <= 'f') {
          return static_cast<unsigned> (digit) - ('a' - 10);
        }
        if (digit >= 'A' && digit <= 'F') {
          return static_cast<unsigned> (digit) - ('A' - 10);
        }
        return static_cast<unsigned> (digit) - '0';
      };
      result +=
        static_cast<char> ((hex2dec (*(pos + 1)) << 4) | hex2dec (*(pos + 2)));
      pos += 2;
    } else {
      result += *pos;
    }
  }
  return result;
}

void normalize (parts& p) {
  if (p.scheme) {
    lowercase (*p.scheme);
  }
  if (p.authority.userinfo) {
    p.authority.userinfo = percent_decode (*p.authority.userinfo);
  }
  if (p.authority.host) {
    lowercase (*p.authority.host);
    p.authority.host = percent_decode (*p.authority.host);
  }
  p.path.remove_dot_segments ();
  for (auto& segment : p.path.segments) {
    segment = percent_decode (segment);
  }
  if (p.query) {
    p.query = percent_decode (*p.query);
  }
  if (p.fragment) {
    p.fragment = percent_decode (*p.fragment);
  }
}

void normalize (std::optional<parts>& p) {
  if (p) {
    normalize (*p);
  }
}

std::ostream& operator<< (std::ostream& os, authority const& auth) {
  if (auth.userinfo) {
    os << *auth.userinfo << "@";
  }
  if (auth.host) {
    os << *auth.host;
  }
  if (auth.port) {
    os << ':' << *auth.port;
  }
  return os;
}

// 5.2.2.  Transform References
//
// -- The URI reference is parsed into the five URI components
// --
// (R.scheme, R.authority, R.path, R.query, R.fragment) = parse(R);
//
// -- A non-strict parser may ignore a scheme in the reference
// -- if it is identical to the base URI's scheme.
// --
// if ((not strict) and (R.scheme == Base.scheme)) then
//    undefine(R.scheme);
// endif;
//
// if defined(R.scheme) then
//    T.scheme    = R.scheme;
//    T.authority = R.authority;
//    T.path      = remove_dot_segments(R.path);
//    T.query     = R.query;
// else
//    if defined(R.authority) then
//       T.authority = R.authority;
//       T.path      = remove_dot_segments(R.path);
//       T.query     = R.query;
//    else
//       if (R.path == "") then
//          T.path = Base.path;
//          if defined(R.query) then
//             T.query = R.query;
//          else
//             T.query = Base.query;
//          endif;
//       else
//          if (R.path starts-with "/") then
//             T.path = remove_dot_segments(R.path);
//          else
//             T.path = merge(Base.path, R.path);
//             T.path = remove_dot_segments(T.path);
//          endif;
//          T.query = R.query;
//       endif;
//       T.authority = Base.authority;
//    endif;
//    T.scheme = Base.scheme;
// endif;
//
// T.fragment = R.fragment;
parts join (parts const& Base, parts const& R) {
  parts T;
  if (R.scheme) {
    T.scheme = R.scheme;
    T.authority = R.authority;
    T.path = R.path;
    T.path.remove_dot_segments ();
    T.query = R.query;
  } else {
    if (R.authority) {
      T.authority = R.authority;
      T.path = R.path;
      T.path.remove_dot_segments ();
      T.query = R.query;
    } else {
      if (R.path.empty ()) {
        T.path = Base.path;
        if (R.query) {
          T.query = R.query;
        } else {
          T.query = Base.query;
        }
      } else {
        if (R.path.absolute) {
          T.path.absolute = true;
          T.path = R.path;
          T.path.remove_dot_segments ();
        } else {
          T.path = merge (Base, R);
          T.path.remove_dot_segments ();
        }
        T.query = R.query;
      }
      T.authority = Base.authority;
    }
    T.scheme = Base.scheme;
  }
  T.fragment = R.fragment;
  return T;
}

std::optional<parts> join (std::string_view Base, std::string_view R) {
  auto base_parts = split (Base);
  if (!base_parts) {
    return {};
  }
  auto reference_parts = split (R);
  if (!reference_parts) {
    return {};
  }
  return join (*base_parts, *reference_parts);
}

std::ostream& compose (std::ostream& os, parts const& p) {
  if (p.scheme) {
    os << *p.scheme << ':';
  }
  if (p.authority) {
    os << "//" << p.authority;
  }
  os << static_cast<std::filesystem::path> (p.path).generic_string ();
  if (p.query) {
    os << '?' << *p.query;
  }
  if (p.fragment) {
    os << '#' << *p.fragment;
  }
  return os;
}

std::string compose (parts const& p) {
  std::stringstream result;
  result << p;
  return result.str ();
}

std::ostream& operator<< (std::ostream& os, parts const& p) {
  return compose (os, p);
}

}  // end namespace uri
