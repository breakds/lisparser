#pragma once

#include <iostream>
#include <memory>
#include "token.h"

namespace lisparser {

class Tokenizer {
 public:
  Tokenizer(const std::string &code)
      : _input_stream(new std::istringstream(code)) {}

  Tokenizer(std::istream *input)
      : _input_stream(input) {}
  
  Token Next();
  
 private:
  Tokenizer(const Tokenizer&) = delete;
  Tokenizer(Tokenizer&&) = delete;
  const Tokenizer &operator=(const Tokenizer&) = delete;
  const Tokenizer &operator=(Tokenizer&&) = delete;
  
  std::unique_ptr<std::istream> _input_stream;
};

}  // namespace lisparser

