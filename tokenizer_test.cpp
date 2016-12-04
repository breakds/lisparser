#include "token.h"
#include "tokenizer.h"

#include "gtest/gtest.h"


namespace lisparser {

TEST(Tokenizer, ParenTest) {
  Tokenizer tokenizer("(( ) )");

  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, KeywordTest) {
  Tokenizer tokenizer("(:a (:Nice-Keyword))");

  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::KEYWORD, ":a"), tokenizer.Next());
  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::KEYWORD, ":nice-keyword"), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, KeywordFailureTest) {
  {
    Tokenizer tokenizer(":,");
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer(":");
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }
}

TEST(Tokenizer, StringTest) {
  {
    Tokenizer tokenizer("\"haha\"");
    ASSERT_EQ(Token(Token::STRING, "haha"), tokenizer.Next());
    ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }

  {
    Tokenizer tokenizer("\"\"");
    ASSERT_EQ(Token(Token::STRING, ""), tokenizer.Next());
    ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }


  {
    Tokenizer tokenizer("(\"ha(h-a\")");
    ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::STRING, "ha(h-a"), tokenizer.Next());
    ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }
}

TEST(Tokenizer, StringEscapeTest) {
  Tokenizer tokenizer("(\"I have space, (\\\\) and \\\"escapes\\\"\")");
  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::STRING, "I have space, (\\) and \"escapes\""),
            tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, CommaTest) {
  Tokenizer tokenizer("Comma, and \",\"");
  ASSERT_EQ(Token(Token::SYMBOL, "comma"), tokenizer.Next());
  ASSERT_EQ(Token(Token::COMMA), tokenizer.Next());
  ASSERT_EQ(Token(Token::SYMBOL, "and"), tokenizer.Next());
  ASSERT_EQ(Token(Token::STRING, ","), tokenizer.Next());
  ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, SymbolTest) {
  {
    Tokenizer tokenizer("(Defmethod a (B \"C\" D))");
    ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::SYMBOL, "defmethod"), tokenizer.Next());
    ASSERT_EQ(Token(Token::SYMBOL, "a"), tokenizer.Next());
    ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::SYMBOL, "b"), tokenizer.Next());
    ASSERT_EQ(Token(Token::STRING, "C"), tokenizer.Next());
    ASSERT_EQ(Token(Token::SYMBOL, "d"), tokenizer.Next());
    ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }

  {
    Tokenizer tokenizer("A1b2C3");
    ASSERT_EQ(Token(Token::SYMBOL, "a1b2c3"), tokenizer.Next());
    ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }
}

TEST(Tokenizer, NumberTest) {
  Tokenizer tokenizer("(12 (11.52))");
  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::INTEGER, "12"), tokenizer.Next());
  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::FLOAT, "11.52"), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  ASSERT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, SpecialNumberTest) {
  Tokenizer tokenizer(".23 -15 a-b -.88");
  ASSERT_EQ(Token(Token::FLOAT, ".23"), tokenizer.Next());
  ASSERT_EQ(Token(Token::INTEGER, "-15"), tokenizer.Next());
  ASSERT_EQ(Token(Token::SYMBOL, "a-b"), tokenizer.Next());
  ASSERT_EQ(Token(Token::FLOAT, "-.88"), tokenizer.Next());
}

TEST(Tokenizer, NumberFailureTest) {
  {
    Tokenizer tokenizer("a . b");
    ASSERT_EQ(Token(Token::SYMBOL, "a"), tokenizer.Next());
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("15.8.9");
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("-");
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("-..");
    ASSERT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }
}

}  // namespace lisparser
