//===- include/uri/uri.hpp --------------------------------*- mode: C++ -*-===//
//*             _  *
//*  _   _ _ __(_) *
//* | | | | '__| | *
//* | |_| | |  | | *
//*  \__,_|_|  |_| *
//*                *
//===----------------------------------------------------------------------===//
//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
// for license information.
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//
#ifndef URI_URI_HPP
#define URI_URI_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "uri/pctdecode.hpp"
#include "uri/pctencode.hpp"

namespace uri {

struct parts {
  struct path {
    bool absolute = false;
    std::vector<std::string_view> segments;

    // Remove dot segments from the path.
    void remove_dot_segments ();
    [[nodiscard]] constexpr bool empty () const noexcept {
      return segments.empty ();
    }
    explicit operator std::string () const;
    explicit operator std::filesystem::path () const;
    bool operator== (path const& rhs) const;
    bool operator!= (path const& rhs) const { return !operator== (rhs); }
  };
  struct authority {
    std::optional<std::string_view> userinfo;
    std::string_view host;
    std::optional<std::string_view> port;

    bool operator== (authority const& rhs) const;
    bool operator!= (authority const& rhs) const { return !operator== (rhs); }
  };

  std::optional<std::string_view> scheme;
  std::optional<struct authority> authority;
  struct path path;
  std::optional<std::string_view> query;
  std::optional<std::string_view> fragment;

  [[nodiscard]] constexpr bool valid () const noexcept {
    return !authority || path.absolute;
  }

  bool operator== (parts const& rhs) const;
  bool operator!= (parts const& rhs) const { return !operator== (rhs); }
};

std::ostream& operator<< (std::ostream& os, struct parts::path const& path);
std::ostream& operator<< (std::ostream& os,
                          struct parts::authority const& auth);
std::ostream& operator<< (std::ostream& os, parts const& p);

std::optional<parts> split (std::string_view in);

parts join (parts const& base, parts const& reference, bool strict = true);
std::optional<parts> join (std::string_view Base, std::string_view R,
                           bool strict = true);

std::string compose (parts const& p);
std::ostream& compose (std::ostream& os, parts const& p);

}  // end namespace uri

#endif  // URI_URI_HPP
