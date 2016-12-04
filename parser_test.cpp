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

  ASSERT_EQ(AST::Vector(AST::Symbol("abc"), AST::Integer(123)),
            *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

TEST(Parser, EmptyListTest) {
  {
    Parser parser("()");

    ASSERT_EQ(AST::Vector(), *parser.Next().value());
    ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
  }

  {
    Parser parser("(()())");

    ASSERT_EQ(AST::Vector(AST::Vector(), AST::Vector()),
              *parser.Next().value());
    ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
  }
}

TEST(Parser, UnmachedParenTest) {
  Parser parser("(() ()");
  ASSERT_EQ(Parser::UNMATCHED_PAREN, parser.Next().error_code());
}

TEST(Parser, TokenizerErrorTest) {
  {
    Parser parser("(,)");
    ASSERT_EQ(Parser::BAD_EVAL_FORM,
              parser.Next().error_code());
  }

  {
    Parser parser("(. )");
    ASSERT_EQ(Parser::TOKENIZER_EXCEPTION,
              parser.Next().error_code());
  }
}

TEST(Parser, EmptyTest) {
  Parser parser("");
  ASSERT_EQ(Parser::EMPTY,
            parser.Next().error_code());
  ASSERT_EQ(Parser::EMPTY,
            parser.Next().error_code());
}

TEST(Parser, CompositeTest) {
  Parser parser("(Hello \"World!\")"
                "(this is a good (:or chance opportunity) to "
                ",get (:+ 1 -.5))");
  
  ASSERT_EQ(AST::Vector(AST::Symbol("hello"), AST::String("World!")),
            *parser.Next().value());

  ASSERT_EQ(AST::Vector(AST::Symbol("this"),
                        AST::Symbol("is"),
                        AST::Symbol("a"),
                        AST::Symbol("good"),
                        AST::Vector(AST::Keyword(":or"),
                                    AST::Symbol("chance"),
                                    AST::Symbol("opportunity")),
                        AST::Symbol("to"),
                        AST::EvalForm("get"),
                        AST::Vector(AST::Keyword(":+"),
                                    AST::Integer(1),
                                    AST::Double(-0.5))),
            *parser.Next().value());
  ASSERT_EQ(Parser::EMPTY, parser.Next().error_code());
}

}  // namespace lisparser
