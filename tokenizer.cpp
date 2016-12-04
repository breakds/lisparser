#include "tokenizer.h"

#include <cctype>
#include "util/char_ops.h"

namespace lisparser {

Token Tokenizer::Next() {

  char character;

  // TODO(breakds): Needs to implement comments handling.
  do {
    int peek = _input_stream->peek();

    // Consume the skippers if encountered.
    if (util::char_ops::Skipper(peek)) {
      _input_stream->get(character);
      continue;
    }
    
    // Standard LL(1) parser pattern with 1-character dispatcher.
    switch (peek) {
      case EOF:
        return MakeToken<Token::TERMINATOR>(_input_stream.get());

      case '(':
        return MakeToken<Token::OPEN_PAREN>(_input_stream.get());
        
      case ')':
        return MakeToken<Token::CLOSE_PAREN>(_input_stream.get());

      case ':':
        return MakeToken<Token::KEYWORD>(_input_stream.get());

      case '"':
        return MakeToken<Token::STRING>(_input_stream.get());

      case ',':
        return MakeToken<Token::COMMA>(_input_stream.get());

      default:
        if (std::isdigit(peek) || peek == '.' || peek == '-') {
          return MakeNumberToken(_input_stream.get());
        } else if (util::char_ops::SymbolCharacter(peek)) {
          return MakeToken<Token::SYMBOL>(_input_stream.get());
        }
        
        return MakeToken<Token::INVALID_TOKEN>(_input_stream.get());
    }
  } while (true);
}

}  // name lisparser
