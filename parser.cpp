#include "parser.h"

#include <fstream>

namespace lisparser {

Parser Parser::FromFile(const std::string &path) {
  return Parser(new Tokenizer(
      new std::ifstream(path, std::ifstream::in)));
}

util::Result<AST> Parser::Next() {

  if (_closed) return util::Result<AST>(Parser::EMPTY);

  Token token = _tokenizer->Next();

  if (token.type == Token::TERMINATOR) {
    _closed = true;
    return util::Result<AST>(Parser::EMPTY);
  }

  return ConsumeToken(std::move(token));
}

util::Result<AST> Parser::ConsumeToken(Token &&start) {
  switch (start.type) {
    case Token::TERMINATOR:
      _closed = true;
      return util::Result<AST>(Parser::EMPTY);

    case Token::INVALID_TOKEN:
      _closed = true;
      return util::Result<AST>(Parser::TOKENIZER_EXCEPTION,
                               std::move(start.value));

    case Token::KEYWORD:
      return AST::Keyword(std::move(start.value));

    case Token::SYMBOL:
      return AST::Symbol(std::move(start.value));

    case Token::STRING:
      return AST::String(std::move(start.value));

    case Token::COMMA: {
      Token token = _tokenizer->Next();

      if (token.type != Token::SYMBOL) {
        return util::Result<AST>(Parser::BAD_EVAL_FORM);
      }
      
      return AST::EvalForm(std::move(token.value));
    }

    case Token::FLOAT: 
      return AST::Double(std::stod(start.value));

    case Token::INTEGER:
      return AST::Integer(std::stoll(start.value));

    case Token::OPEN_PAREN: {
      AST ast = AST::Vector();

      do {
        Token token = _tokenizer->Next();
        if (token.type == Token::CLOSE_PAREN) {
          return std::move(ast);
        }

        auto result = ConsumeToken(std::move(token));

        if (!result.ok()) {
          if (result.error_code() == Parser::EMPTY) {
            return util::Result<AST>(Parser::UNMATCHED_PAREN);
          }
          return result;
        }

        ast.Push(std::move(*result.value()));
      } while(true);

      return util::Result<AST>(Parser::TOKENIZER_EXCEPTION,
                               "Unreachable.");
    }
      
    default:
      // TODO(breakds): Append the stringified token to the end of the
      // error message.
      return util::Result<AST>(Parser::TOKENIZER_EXCEPTION,
                               "Unrecognizable token.");
  }
}

}  // namespace lisparser
