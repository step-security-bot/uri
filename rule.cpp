#include "rule.hpp"

rule rule::join_rule (matched_result::value_type const& m) {
  auto const& [head, acceptors] = m;
  return {tail_->substr (head.length ()), join (acceptors_, acceptors)};
}
