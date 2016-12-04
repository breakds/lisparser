#pragma once

#include <cassert>
#include <sstream>
#include <string>

namespace lisparser {

struct Token {
  enum Type {
    INVALID_TOKEN = 1000,
    TERMINATOR = 1001,
    OPEN_PAREN = 0,
    CLOSE_PAREN = 1,
    COMMA = 2,
    KEYWORD = 3,
    SYMBOL = 4,
    STRING = 5,
    FLOAT = 6,
    INTEGER = 7,
  };

  Token(Type input_type, const std::string &input_value)
      : type(input_type), value(input_value) {}

  Token(Type input_type, std::string &&input_value)
      : type(input_type), value(input_value) {}

  Token(Type input_type)
      : type(input_type), value() {}

  inline bool operator==(const Token& other) const {
    return (type == other.type) && (value == other.value);
  }

  Type type;
  std::string value;
};

// For debug purpose.
std::ostream &operator<<(std::ostream &output, const Token &token);

// MakeToken is the LL(1) dispatcher functions for those tokens. It is
// fully specialized for each token type below.
template <Token::Type token_type>
Token MakeToken(std::istream *stream) {
  return Token(Token::INVALID_TOKEN);
}

// Fully specialized functions goes to the .cpp file for their
// implementation to avoid multiple definition.
template <> Token MakeToken<Token::TERMINATOR>(std::istream *stream);
template <> Token MakeToken<Token::OPEN_PAREN>(std::istream *stream);
template <> Token MakeToken<Token::CLOSE_PAREN>(std::istream *stream);
template <> Token MakeToken<Token::COMMA>(std::istream *stream);
template <> Token MakeToken<Token::KEYWORD>(std::istream *stream);
template <> Token MakeToken<Token::SYMBOL>(std::istream *stream);
template <> Token MakeToken<Token::STRING>(std::istream *stream);
template <> Token MakeToken<Token::INVALID_TOKEN>(std::istream *stream);

Token MakeNumberToken(std::istream *stream);

}  // namespace lisparser

