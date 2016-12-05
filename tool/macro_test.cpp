#include "tool/macro.h"

#include <string>
#include "gtest/gtest.h"
#include "parser.h"

namespace lisparser {
namespace macro {

AST ParseOrDie(const std::string &code) {
  Parser parser(code);
  return std::move(*parser.Next().value());
}

TEST(Macro, AcquireTest) {
  Engine engine;

  engine.Acquire(AST::Vector(
      AST::Symbol("defmacro"),
      AST::Keyword(":something"),
      AST::Vector(
          AST::Symbol("a"),
          AST::Symbol("b")),
      AST::Vector(
          AST::Symbol("+"),
          AST::EvalForm("a"),
          AST::EvalForm("b"))));

  EXPECT_EQ(1, engine.size());
}

TEST(Macro, SimpleEvalTest) {
  Engine engine;

  // Parse Macro
  EXPECT_TRUE(engine.Acquire(ParseOrDie(
      "(defmacro :plus (a b)"
      "  (+ ,a ,b))")).ok());

  EXPECT_EQ(ParseOrDie("(+ (+ 12 13) (+ 11.5 11.6))"),
            *engine.Evaluate(
                ParseOrDie("(:plus (:plus 12 13) "
                           "       (:plus 11.5 11.6))")).value());
}

TEST(Macro, HierarchicalMacroTest) {
  Engine engine;

  // Parse Macros
  EXPECT_TRUE(engine.Acquire(ParseOrDie(
      "(defmacro :plus (a b)"
      "  (+ ,a ,b))")).ok());

  EXPECT_TRUE(engine.Acquire(ParseOrDie(
      "(defmacro :multi-plus (a b c d)"
      "  (:plus ,a (:plus ,b (:plus ,c ,d))))")).ok());
  
  EXPECT_EQ(
      ParseOrDie("(the result is (+ 1 (+ 2 (+ 3 (+ 4 5)))))"),
      *engine.Evaluate(
          ParseOrDie("(the result is "
                     "  (:multi-plus 1 2 3 (:plus 4 5)))")).value());
}

}  // namespace macro
}  // namespace lisparser
