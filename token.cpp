#include "token.h"

#include <cctype>
#include <iostream>
#include "util/char_ops.h"

namespace lisparser {

std::ostream &operator<<(std::ostream &output,
                         const Token &token) {
  output << "{" << token.type << ", " << token.value << "}";
  return output;
}

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
Token MakeToken<Token::COMMA>(std::istream *stream) {
  char character;

  assert(stream->get(character));
  assert(character == ',');

  return Token(Token::COMMA);
}

template <>
Token MakeToken<Token::KEYWORD>(std::istream *stream) {
  char character;

  assert(stream->get(character));
  assert(character == ':');

  std::string value = ":";
  while (util::char_ops::SymbolCharacter(stream->peek())) {
    stream->get(character);
    value.push_back(static_cast<char>(std::tolower(character)));
  }

  if (value.size() == 1) {
    return Token(Token::INVALID_TOKEN,
                 "Empty keyword with single colon.");
  }
  
  return Token(Token::KEYWORD, std::move(value));
}

template <>
Token MakeToken<Token::SYMBOL>(std::istream *stream) {
  char character;

  std::string value;
  while (util::char_ops::SymbolCharacter(stream->peek())) {
    stream->get(character);
    value.push_back(static_cast<char>(std::tolower(character)));
  }

  assert(!value.empty());

  return Token(Token::SYMBOL, std::move(value));
}

template <>
Token MakeToken<Token::STRING>(std::istream *stream) {
  char character;

  assert(stream->get(character));
  assert(character == '"');

  bool escape_sign = false;
  std::string value;
  do {
    int peek = stream->peek();

    if (escape_sign) {
      switch (peek) {
        case '"':
          stream->get(character);
          value.push_back('"');
          break;
          
        case '\\':
          stream->get(character);
          value.push_back('\\');
          break;

        default:
          return Token(Token::INVALID_TOKEN,
                       "Invalid escape character in string.");
      }

      escape_sign = false;
    } else {
      switch (peek) {
        case EOF:
          return Token(Token::INVALID_TOKEN,
                       "Unclosed string: end-of-file reached.");
        case '"':
          stream->get(character);
          return Token(Token::STRING, std::move(value));

        case '\\':
          stream->get(character);
          escape_sign = true;
          break;

        default:
          stream->get(character);
          value.push_back(character);
      }
    }
  } while(true);
}


Token MakeNumberToken(std::istream *stream) {
  char character;

  bool dot = false;

  std::string value;
  do {
    int peek = stream->peek();

    if (peek == EOF) {
      break;
    } else if (std::isdigit(peek)) {
      stream->get(character);
      value.push_back(character);
    } else if (peek == '.') {
      if (dot) return Token(Token::INVALID_TOKEN,
                            "Number with more than one dot.");
      dot = true;
      stream->get(character);
      value.push_back(character);
    } else {
      break;
    }
  } while (true);

  assert(!value.empty());

  if (value.size() == 1 && value[0] == '.') {
    return Token(Token::INVALID_TOKEN, "Number with nothing but one dot.");
  }

  if (dot) {
    return Token(Token::FLOAT, std::move(value));
  } else {
    return Token(Token::INTEGER, std::move(value));
  }
}

template <>
Token MakeToken<Token::INVALID_TOKEN>(std::istream *stream) {
  char character;
  assert(stream->get(character));
  return Token(Token::INVALID_TOKEN, std::string(1, character));
}

}  // namespace lisparser
