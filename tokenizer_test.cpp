#include "token.h"
#include "tokenizer.h"

#include "gtest/gtest.h"


namespace lisparser {

TEST(Tokenizer, ParenTest) {
  Tokenizer tokenizer("(( ) )");

  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, KeywordTest) {
  Tokenizer tokenizer("(:a (:Nice-Keyword))");

  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::KEYWORD, ":a"), tokenizer.Next());
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::KEYWORD, ":nice-keyword"), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, KeywordFailureTest) {
  {
    Tokenizer tokenizer(":,");
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer(":");
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }
}

TEST(Tokenizer, StringTest) {
  {
    Tokenizer tokenizer("\"haha\"");
    EXPECT_EQ(Token(Token::STRING, "haha"), tokenizer.Next());
    EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }

  {
    Tokenizer tokenizer("\"\"");
    EXPECT_EQ(Token(Token::STRING, ""), tokenizer.Next());
    EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }

  {
    Tokenizer tokenizer("(\"ha(h-a\")");
    EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::STRING, "ha(h-a"), tokenizer.Next());
    EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }
}

TEST(Tokenizer, StringEscapeTest) {
  Tokenizer tokenizer("(\"I have space, (\\\\) and \\\"escapes\\\"\")");
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::STRING, "I have space, (\\) and \"escapes\""),
            tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, CommaTest) {
  Tokenizer tokenizer("Comma, and \",\"");
  EXPECT_EQ(Token(Token::SYMBOL, "comma"), tokenizer.Next());
  EXPECT_EQ(Token(Token::COMMA), tokenizer.Next());
  EXPECT_EQ(Token(Token::SYMBOL, "and"), tokenizer.Next());
  EXPECT_EQ(Token(Token::STRING, ","), tokenizer.Next());
  EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, SymbolTest) {
  {
    Tokenizer tokenizer("(Defmethod a (B \"C\" D))");
    EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::SYMBOL, "defmethod"), tokenizer.Next());
    EXPECT_EQ(Token(Token::SYMBOL, "a"), tokenizer.Next());
    EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::SYMBOL, "b"), tokenizer.Next());
    EXPECT_EQ(Token(Token::STRING, "C"), tokenizer.Next());
    EXPECT_EQ(Token(Token::SYMBOL, "d"), tokenizer.Next());
    EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
    EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }

  {
    Tokenizer tokenizer("A1b2C3");
    EXPECT_EQ(Token(Token::SYMBOL, "a1b2c3"), tokenizer.Next());
    EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
  }
}

TEST(Tokenizer, NumberTest) {
  Tokenizer tokenizer("(12 (11.52))");
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::INTEGER, "12"), tokenizer.Next());
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::FLOAT, "11.52"), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::CLOSE_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::TERMINATOR), tokenizer.Next());
}

TEST(Tokenizer, SpecialNumberTest) {
  Tokenizer tokenizer(".23 -15 a-b (-.88");
  EXPECT_EQ(Token(Token::FLOAT, ".23"), tokenizer.Next());
  EXPECT_EQ(Token(Token::INTEGER, "-15"), tokenizer.Next());
  EXPECT_EQ(Token(Token::SYMBOL, "a-b"), tokenizer.Next());
  EXPECT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
  EXPECT_EQ(Token(Token::FLOAT, "-.88"), tokenizer.Next());
}

TEST(Tokenizer, NumberFailureTest) {
  {
    Tokenizer tokenizer("a . b");
    EXPECT_EQ(Token(Token::SYMBOL, "a"), tokenizer.Next());
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("15.8.9");
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("-");
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }

  {
    Tokenizer tokenizer("-..");
    EXPECT_EQ(Token::INVALID_TOKEN, tokenizer.Next().type);
  }
}

}  // namespace lisparser
