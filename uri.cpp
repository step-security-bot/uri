#include "uri.hpp"

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

auto host_rule (uri_parts& result) {
  // host          = IP-literal / IPv4address / reg-name
  return [&result] (rule const& r) {
    return r
      .concat (
        [] (rule const& r1) {
          return r1.alternative (ip_literal, ipv4address, reg_name)
            .matched ("IP-literal / IPv4address / reg-name", r1);
        },
        [&result] (std::string_view host) { result.host = host; })
      .matched ("host", r);
  };
}

auto userinfo_at (uri_parts& result) {
  // userinfo-at = userinfo "@"
  return [&result] (rule const& r) {
    return r
      .concat (userinfo,
               [&result] (std::string_view const userinfo) {
                 result.userinfo = userinfo;
               })
      .concat (commercial_at)
      .matched ("userinfo \"@\"", r);
  };
}

// port = *DIGIT
auto port (rule const& r) {
  return r.star (digit).matched ("port", r);
}

auto colon_port (uri_parts& result) {
  // colon-port = ":" port
  return [&result] (rule const& r) {
    return r.concat (colon)
      .concat (port, [&result] (std::string_view const p) { result.port = p; })
      .matched ("\":\" port", r);
  };
}

auto authority (uri_parts& result) {
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
#if 0
  auto segment_nz_nc (rule const& r) {
    return r
      .star (
        [] (rule const& r2) {
          return r2
            .alternative (unreserved, pct_encoded, sub_delims,
                          commercial_at)
            .matched ("unreserved / pct-encoded / sub-delims / \"@\"", r2);
        },
        1U)
      .matched ("segment-nz-nc", r);
  }
#endif

auto path_abempty (uri_parts& result) {
  // path-abempty  = *( "/" segment )
  return [&result] (rule const& r) {
    return r
      .star ([&result] (rule const& r2) {
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

auto path_absolute (uri_parts& result) {
  // path-absolute = "/" [ segment-nz *( "/" segment ) ]
  return [&result] (rule const& r) {
    return r
      .concat (solidus,
               [&result] (std::string_view const seg) {
                 result.segments.emplace_back (seg);
               })
      .optional ([&result] (rule const& r1) {
        return r1
          .concat (segment_nz,
                   [&result] (std::string_view seg) {
                     result.segments.back () += seg;
                   })
          .concat ([&result] (rule const& r2) {
            return r2
              .star ([&result] (rule const& r3) {
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
auto path_empty (rule const& r) {
  return r.star (pchar, 0, 0).matched ("path-empty", r);
}

// path-rootless = segment-nz *( "/" segment )
auto path_rootless (uri_parts& result) {
  return [&result] (rule const& r) {
    return r
      .concat (segment_nz,
               [&result] (std::string_view const seg) {
                 result.segments.emplace_back (seg);
               })
      .star ([&result] (rule const& r1) {
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
auto hier_part (uri_parts& result) {
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

// query         = *( pchar / "/" / "?" )
auto query (rule const& r) {
  return r
    .star ([] (rule const& r2) {
      return r2.alternative (pchar, solidus, question_mark)
        .matched (R"(pchar / "/" / "?")", r2);
    })
    .matched ("query", r);
}
// fragment      = *( pchar / "/" / "?" )
auto fragment (rule const& r) {
  return query (r);
}

}  // end anonymous namespace

// URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
std::optional<uri_parts> uri_rule (std::string_view const in) {
  uri_parts result;
  bool success =
    rule{in}
      .concat (
        scheme,
        [&result] (std::string_view const scheme) { result.scheme = scheme; })
      .concat (colon)
      .concat (hier_part (result))
      .optional ([&result] (rule const& rq) {
        // "?" query
        return rq
          .concat (question_mark)  // "?"
          .concat (
            query,  // query
            [&result] (std::string_view const query) { result.query = query; })
          .matched ("\"?\" query", rq);
      })
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
      .done ();
  if (success) {
    return result;
  }
  return {};
}
