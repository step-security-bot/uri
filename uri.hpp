#ifndef URI_HPP
#define URI_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace uri {

using path_container = std::vector<std::string>;

struct parts {
  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  std::vector<std::string> path;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

std::optional<parts> split (std::string_view in);

path_container remove_dot_segments (path_container const& path);

std::string percent_decode (std::string_view src);

void normalize (parts& p);
void normalize (std::optional<parts>& p);

}  // end namespace uri

#endif  // URI_HPP
