#include "token.h"

namespace lisparser {

template <>
Token MakeToken<Token::TERMINATOR>(std::istream *stream) {
  return Token(Token::TERMINATOR);
}

template <>
Token MakeToken<Token::OPEN_PAREN>(std::istream *stream) {
  char character;

  assert(stream->get(character));
  assert(character == '(');

  return Token(Token::OPEN_PAREN);
}

template <>
Token MakeToken<Token::CLOSE_PAREN>(std::istream *stream) {
  char character;

  assert(stream->get(character));
  assert(character == ')');

  return Token(Token::CLOSE_PAREN);
}

template <>
Token MakeToken<Token::INVALID_TOKEN>(std::istream *stream) {
  char character;
  assert(stream->get(character));
  return Token(Token::INVALID_TOKEN, std::string(1, character));
}

}  // namespace lisparser
