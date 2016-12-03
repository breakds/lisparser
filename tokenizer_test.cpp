#include "token.h"
#include "tokenizer.h"

#include "gtest/gtest.h"


namespace lisparser {

TEST(Tokenizer, SimpleTest) {
  Tokenizer tokenizer("(())()");

  ASSERT_EQ(Token(Token::OPEN_PAREN), tokenizer.Next());
}

}  // namespace lisparser
