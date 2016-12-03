#pragma once

#include <iostream>
#include <memory>
#include "token.h"

namespace lisparser {

class Tokenizer {
 public:
  Tokenizer(const std::string &code)
      : _input_stream(new std::istringstream(code)) {}
  
  Token Next();
  
 private:
  std::unique_ptr<std::istream> _input_stream;
};

}  // namespace lisparser

