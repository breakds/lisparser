#include "parser.h"

#include "gtest/gtest.h"

namespace lisparser {

TEST(Parser, KeywordTest) {
  Parser parser(":abc");
  ASSERT_EQ(AST::Keyword(":abc"), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, SymbolTest) {
  Parser parser("AbC");
  ASSERT_EQ(AST::Symbol("abc"), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, StringTest) {
  Parser parser("\"AbC\"");
  ASSERT_EQ(AST::String("AbC"), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, EvalFormTest) {
  Parser parser(",VAR");
  ASSERT_EQ(AST::EvalForm("var"), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, IntegerTest) {
  Parser parser("123");
  ASSERT_EQ(AST::Integer(123), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, DoubleTest) {
  Parser parser("123.567");
  ASSERT_EQ(AST::Double(123.567), *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, ListTest) {
  Parser parser("(abc 123)");

  AST expected = AST::Vector();
  expected.Push(AST::Symbol("abc"));
  expected.Push(AST::Integer(123));
  ASSERT_EQ(expected, *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}


}
