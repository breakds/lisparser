#include "tokenizer.h"

namespace lisparser {

Token Tokenizer::Next() {

  int character;

  do {
    // Standard LL(1) parser pattern with 1-character dispatcher.
    switch (_input_stream->peek()) {
      case EOF:
        return MakeToken<Token::TERMINATOR>(_input_stream.get());

      case ' ':
      case 13:
      case 10:
        continue;
        
      case '(':
        return MakeToken<Token::OPEN_PAREN>(_input_stream.get());
        
      case ')':
        return MakeToken<Token::CLOSE_PAREN>(_input_stream.get());
        
      default:
        return MakeToken<Token::INVALID_TOKEN>(_input_stream.get());
    }
  } while (true);
}

}  // name lisparser
