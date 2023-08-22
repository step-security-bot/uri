#include "rule.hpp"

#include <iomanip>
#include <iostream>

bool rule::done () const {
  if (!tail_ || !tail_->empty ()) {
    return false;
  }
  std::for_each (std::begin (acceptors_), std::end (acceptors_),
                 [] (acceptor_container::value_type const& a) {
                   std::get<0> (a) (std::get<1> (a));
                 });
  return true;
}

rule::matched_result rule::matched (char const* name, rule const& in) const {
  assert (!tail_ || in.tail_);
  static constexpr auto trace = false;
  if (trace) {
    std::cout << (tail_ ? '*' : '-') << ' ' << std::quoted (name);
  }

  if (tail_) {
    std::string_view const str =
      in.tail_->substr (0, in.tail_->length () - tail_->length ());
    if (trace) {
      std::cout << ' ' << std::quoted (str) << '\n';
    }
    return std::make_tuple (str, acceptors_);
  }

  if (trace) {
    std::cout << '\n';
  }
  return {};
}
