#ifndef URI_HPP
#define URI_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace uri {

using path_container = std::vector<std::string>;

struct path_description {
  bool absolute = false;
  bool directory = false;
  path_container segments;

  // Remove dot segments from the string.
  //
  // See also Section 5.2.4 of RFC 3986.
  // http://tools.ietf.org/html/rfc3986#section-5.2.4
  void remove_dot_segments ();
  explicit operator std::filesystem::path () const;
};

struct parts {
  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  path_description path;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

std::optional<parts> split (std::string_view in);

std::string percent_decode (std::string_view src);

void normalize (parts& p);
void normalize (std::optional<parts>& p);

}  // end namespace uri

#endif  // URI_HPP
