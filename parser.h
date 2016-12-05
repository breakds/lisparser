#pragma once

#include <memory>
#include "ast.h"
#include "tokenizer.h"
#include "util/result.h"

namespace lisparser {

class Parser {
 public:
  enum ParserError {
    EMPTY = 1,
    TOKENIZER_EXCEPTION = 2,
    BAD_EVAL_FORM = 3,
    UNMATCHED_PAREN = 4,
  };

  Parser(Tokenizer *tokenizer)
      : _tokenizer(tokenizer), _closed(false) {}

  Parser(const std::string &code)
      : Parser(new Tokenizer(code)) {}

  Parser(Parser &&other) 
      : _tokenizer(std::move(other._tokenizer)),
        _closed(other._closed) {}

  static Parser FromFile(const std::string &path);

  util::Result<AST> Next();

 private:
  Parser(const Parser&) = delete;
  const Parser &operator=(const Parser&) = delete;
  const Parser &operator=(Parser&&) = delete;
  
  util::Result<AST> ConsumeToken(Token &&start);
  
  std::unique_ptr<Tokenizer> _tokenizer;
  bool _closed;
};

}  // namespace lisparser
