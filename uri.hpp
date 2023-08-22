#ifndef URI_HPP
#define URI_HPP

#include <string>

#include "rule.hpp"

namespace uri {

struct parts {
  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  std::vector<std::string> segments;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

std::optional<parts> split (std::string_view in);

}  // end namespace uri

#endif  // URI_HPP
