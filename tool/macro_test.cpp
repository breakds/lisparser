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

TEST(Macro, NoArgumentEvalTest) {
  Engine engine;

  // Parse Macro
  EXPECT_TRUE(engine.Acquire(ParseOrDie(
      "(defmacro :laugh () \"haha\")")).ok());

  EXPECT_EQ(ParseOrDie("(i say \"haha\")"),
            *engine.Evaluate(
                ParseOrDie("(I say (:laugh))")).value());
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

TEST(Macro, AcquireFailureArgumentNotSymbol) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie(
      "(defmacro :plus (a 17)"
      "  (+ ,a 3))"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_ARGS, result.error_code());
  EXPECT_EQ("17 is not a valid macro argument in macro [:plus]",
            result.error_message());
}

TEST(Macro, AcquireFailureArgumentNotList) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie(
      "(defmacro :hehe :haha ())"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("macro arguments should be a list in macro [:hehe]",
            result.error_message());
}

TEST(Macro, AcquireFailureDuplicateArgument) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie(
      "(defmacro :plus (a a)"
      "  (+ ,a ,a))"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_ARGS, result.error_code());
  EXPECT_EQ("duplicate argument 'a' in macro [:plus]",
            result.error_message());
}

TEST(Macro, AcquireFailureAtomForm) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie("abc"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("macro defintion should be in list form",
            result.error_message());
}

TEST(Macro, AcquireFailureNoDefmacro) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie("(abc a b c)"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("macro definition should start with 'defmacro'",
            result.error_message());
}

TEST(Macro, AcquireFailureLengthFour) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie("(defmacro)"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("defmacro form should be of length 4",
            result.error_message());
}

TEST(Macro, AcquireFailureNameNotKeyword) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie(
      "(defmacro invalid-name () ())"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("macro name should be a keyword",
            result.error_message());
}

TEST(Macro, AcquireFailureUndefinedArgument) {
  Engine engine;

  // Parse Macros
  auto result = engine.Acquire(ParseOrDie(
      "(defmacro :plus (a b) ,c)"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(INVALID_MACRO_FORM, result.error_code());
  EXPECT_EQ("',c' is not in the lambda list in macro [:plus]",
            result.error_message());
}

TEST(Macro, EvaluateFailureSignatureMismatch) {
  Engine engine;

  // Parse Macro
  EXPECT_TRUE(engine.Acquire(ParseOrDie(
      "(defmacro :test (a b) (c ,a ,b))")).ok());


  auto result = engine.Evaluate(ParseOrDie(
      "(a (:test 1) b)"));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(SIGNATURE_MISMATCH, result.error_code());
  EXPECT_EQ(":test wanted 2 arguments, but 1 provided",
            result.error_message());

}

}  // namespace macro
}  // namespace lisparser
