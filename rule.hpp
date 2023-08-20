#ifndef RULE_HPP
#define RULE_HPP

#include <array>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

class rule {
public:
  using optsv = std::optional<std::string_view>;
  using acceptor = std::function<void (std::string_view)>;
  using acceptor_container =
    std::vector<std::tuple<acceptor, std::string_view>>;

private:
  rule (optsv string, acceptor_container&& acceptors)
      : tail_{string}, acceptors_{std::move (acceptors)} {}
  rule (optsv string, acceptor_container const& acceptors)
      : tail_{string}, acceptors_{acceptors} {}
  rule () = default;

public:
  explicit rule (std::string_view string) : tail_{string} {}
  rule (rule const& rhs) = default;
  rule (rule&& rhs) noexcept = default;
  ~rule () noexcept = default;

  rule& operator= (rule const& rhs) = default;
  rule& operator= (rule&& rhs) noexcept = default;

  bool done () {
    if (!tail_ || !tail_->empty ()) {
      return false;
    }
    std::for_each (std::begin (acceptors_), std::end (acceptors_),
                   [] (acceptor_container::value_type const& a) {
                     std::get<0> (a) (std::get<1> (a));
                   });
    return true;
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule concat (MatchFunction match, AcceptFunction accept) {
    return concat_impl (match, accept, false);
  }

  static void accept_nop (std::string_view) { /* do nothing */
  }

  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule concat (MatchFunction match) {
    return concat (match, rule::accept_nop);
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule optional (MatchFunction match, AcceptFunction accept) {
    if (!tail_) {
      // If matching has already failed, then pass that condition down the
      // chain.
      return *this;
    }
    rule res = rule{*tail_}.concat_impl (match, accept, true);
    if (!res.tail_) {
      return *this;  // The rule failed, so carry on as if nothing happened.
    }
    return rule{res.tail_, join (acceptors_, res.acceptors_)};
  }
  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule optional (MatchFunction match) {
    return optional (match, rule::accept_nop);
  }

  // Variable Repetition:  *Rule
  //
  // The operator "*" preceding an element indicates repetition.  The full form
  // is:
  //
  //   <a>*<b>element
  //
  // where <a> and <b> are optional decimal values, indicating at least <a> and
  // at most <b> occurrences of the element.
  //
  // Default values are 0 and infinity so that *<element> allows any number,
  // including zero; 1*<element> requires at least one; 3*3<element> allows
  // exactly 3 and 1*2<element> allows one or two.
  template <typename MatchFunction, typename = std::enable_if_t<
                                      std::is_invocable_v<MatchFunction, rule>>>
  rule star (MatchFunction match, unsigned min = 0,
             unsigned max = std::numeric_limits<unsigned>::max ());

  rule alternative () { return rule{}; }
  template <typename MatchFunction, typename... Rest>
  rule alternative (MatchFunction match, Rest&&... rest);

  constexpr optsv tail () const { return tail_; }

  using matched_result =
    std::optional<std::tuple<std::string_view, acceptor_container>>;

  matched_result matched (char const* name, rule const& in) const {
    assert (!tail_ || in.tail_);
    static constexpr auto trace = false;
    if (trace) {
      std::cout << ((tail_ && in.tail_) ? '*' : '-') << ' '
                << std::quoted (name);
    }

    if (tail_ && in.tail_) {
      std::string_view const& intail = *in.tail_;
      std::string_view const str =
        intail.substr (0, intail.length () - tail_->length ());
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

  template <typename Predicate>
  matched_result single_char (Predicate pred) const;
  matched_result single_char (char const c) const {
    return single_char ([c2 = std::tolower (static_cast<int> (c))] (char d) {
      return c2 == std::tolower (static_cast<int> (d));
    });
  }

private:
  template <typename MatchFunction, typename AcceptFunction>
  rule concat_impl (MatchFunction match, AcceptFunction accept, bool optional);

  rule alternative_impl () { return rule{}; }
  template <typename MatchFunction, typename... Rest>
  rule alternative_impl (MatchFunction match, Rest&&... rest);

  rule join_rule (matched_result::value_type const& m);

  template <typename T>
  static std::vector<T> join (std::vector<T> const& a,
                              std::vector<T> const& b) {
    std::vector<T> res;
    res.reserve (a.size () + b.size ());
    res.insert (res.end (), a.begin (), a.end ());
    res.insert (res.end (), b.begin (), b.end ());
    return res;
  }

  acceptor_container acceptors_;
  optsv tail_;
};

inline auto single_char (char const first) {
  return [=] (rule const r) { return r.single_char (first); };
}
inline auto char_range (char const first, char const last) {
  return [f = std::tolower (static_cast<int> (first)),
          l = std::tolower (static_cast<int> (last))] (rule const r) {
    return r.single_char ([=] (char const c) {
      auto const cl = std::tolower (static_cast<int> (c));
      return cl >= f && cl <= l;
    });
  };
}

inline auto alpha (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isalpha (static_cast<int> (c)); });
}
inline auto digit (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isdigit (static_cast<int> (c)); });
}
inline auto hexdig (rule const r) {
  return r.single_char (
    [] (char const c) { return std::isxdigit (static_cast<int> (c)); });
}

inline auto commercial_at (rule const r) {
  return r.single_char ('@');
}
inline auto colon (rule const r) {
  return r.single_char (':');
}
inline auto hash (rule const r) {
  return r.single_char ('#');
}
inline auto plus (rule const r) {
  return r.single_char ('+');
}
inline auto minus (rule const r) {
  return r.single_char ('-');
}
inline auto solidus (rule const r) {
  return r.single_char ('/');
}
inline auto question_mark (rule const r) {
  return r.single_char ('?');
}
inline auto full_stop (rule const r) {
  return r.single_char ('.');
}
inline auto left_square_bracket (rule const r) {
  return r.single_char ('[');
}
inline auto right_square_bracket (rule const r) {
  return r.single_char (']');
}

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename... Rest>
rule rule::alternative_impl (MatchFunction match, Rest&&... rest) {
  if (matched_result const m = match (*this)) {
    return join_rule (*m);
  }
  // This didn't match, so try the next one.
  return this->alternative_impl (std::forward<Rest> (rest)...);
}

// star
// ~~~~
template <typename MatchFunction, typename>
rule rule::star (MatchFunction const match, unsigned const min,
                 unsigned const max) {
  if (!tail_) {
    return *this;
  }
  auto length = std::string_view::size_type{0};
  std::string_view str = *tail_;
  auto count = 0U;
  // acceptor_container acceptors = acceptors_;
  for (;;) {
    matched_result const m = match (rule{str});
    if (!m) {
      break;  // No match so no more repetitions.
    }
    ++count;
    if (count > max) {
      break;
    }
    auto const l = std::get<std::string_view> (*m).length ();
    str.remove_prefix (l);
    length += l;

    auto const& a = std::get<acceptor_container> (*m);
    acceptors_.insert (acceptors_.end (), a.begin (), a.end ());
  }
  if (count < min) {
    return rule{};
  }

  return rule{tail_->substr (length), acceptors_};
}

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename... Rest>
rule rule::alternative (MatchFunction match, Rest&&... rest) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  auto r = rule{*tail_}.alternative_impl (match, std::forward<Rest> (rest)...);
  if (!r.tail_) {
    return rule{};
  }
  return rule{r.tail_, join (acceptors_, r.acceptors_)};
}

// concat impl
// ~~~~~~~~~~~
template <typename MatchFunction, typename AcceptFunction>
rule rule::concat_impl (MatchFunction match, AcceptFunction accept,
                        bool optional) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (matched_result m = match (rule{*tail_})) {
    std::get<acceptor_container> (*m).emplace_back (
      accept, std::get<std::string_view> (*m));
    return join_rule (*m);
  }
  if (optional) {
    return *this;
  }
  return rule{};  // Matching failed: yield nothing or failure.
}

// single char
// ~~~~~~~~~~~
template <typename Predicate>
auto rule::single_char (Predicate const pred) const -> matched_result {
  auto const sv = this->tail ();
  if (sv && !sv->empty () && pred (sv->front ())) {
    return std::make_tuple (sv->substr (0, 1), acceptor_container{});
  }
  return {};
}

#endif  // RULE_HPP
