#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <vector>

#include "rule.hpp"

struct uri_parts {
  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  std::vector<std::string> segments;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

std::optional<uri_parts> uri_rule (std::string_view in);

#endif /* uri_h */
