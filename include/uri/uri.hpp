//===- include/uri/uri.hpp --------------------------------*- mode: C++ -*-===//
//*             _  *
//*  _   _ _ __(_) *
//* | | | | '__| | *
//* | |_| | |  | | *
//*  \__,_|_|  |_| *
//*                *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
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
    [[nodiscard]] bool empty () const noexcept { return segments.empty (); }
    [[nodiscard]] bool valid () const noexcept;
    explicit operator std::string () const;
    explicit operator std::filesystem::path () const;
    bool operator== (path const& rhs) const;
    bool operator!= (path const& rhs) const { return !operator== (rhs); }
  };
  struct authority {
    std::optional<std::string_view> userinfo;
    std::string_view host;
    std::optional<std::string_view> port;

    [[nodiscard]] bool valid () const noexcept;

    bool operator== (authority const& rhs) const;
    bool operator!= (authority const& rhs) const { return !operator== (rhs); }
  };

  std::optional<std::string_view> scheme;
  std::optional<struct authority> authority;
  struct path path;
  std::optional<std::string_view> query;
  std::optional<std::string_view> fragment;

  [[nodiscard]] bool valid () const noexcept;

  /// If an authority instance is present, return it otherwise an instance is
  /// created and returned.
  struct authority& ensure_authority () {
    return authority.has_value () ? *authority : authority.emplace ();
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
