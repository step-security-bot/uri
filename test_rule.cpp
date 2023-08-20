#include <gmock/gmock.h>

#include <string>
#include <vector>

#include "rule.hpp"

using testing::ElementsAre;

class Rule : public testing::Test {
protected:
  std::vector<std::string> output_;

  auto remember () {
    return [this] (std::string_view str) { output_.emplace_back (str); };
  }
};
// NOLINTEXTLINE
TEST_F (Rule, Concat) {
  bool ok = rule ("ab")
              .concat ([] (rule r) { return r.single_char ('a'); }, remember ())
              .concat ([] (rule r) { return r.single_char ('b'); }, remember ())
              .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output_, ElementsAre ("a", "b"));
}
// NOLINTEXTLINE
TEST_F (Rule, FirstAlternative) {
  bool ok =
    rule ("ab")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output_, ElementsAre ("a", "b"));
}
// NOLINTEXTLINE
TEST_F (Rule, SecondAlternative) {
  bool ok =
    rule ("ac")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output_, ElementsAre ("a", "c"));
}
// NOLINTEXTLINE
TEST_F (Rule, AlternativeFail) {
  bool ok =
    rule ("ad")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_FALSE (ok);
  EXPECT_TRUE (output_.empty ());
}

TEST_F (Rule, Star) {
  bool ok =
    rule ("aaa")
      .star ([this] (rule r) {
        return r.concat (single_char ('a'), remember ()).matched ("a", r);
      })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output_, ElementsAre ("a", "a", "a"));
}

void test_star2 () {
  std::vector<std::string> output;
  int ctr = 0;
  auto remember = [&output] (std::string_view str) {
    output.emplace_back (str);
  };

  bool ok =
    rule ("/")
      .star ([&remember] (rule r1) {
        return r1.concat (single_char ('/'), remember)
          .concat (
            [] (rule r2) {
              return r2.star (char_range ('a', 'z')).matched ("a-z", r2);
            },
            remember)
          .matched ("*(a-z)", r1);
      })
      .done ();
  assert (output.size () == 2);
  assert (output[0] == "/");
  assert (output[1] == "");
}
