#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "ast.h"
#include "util/result.h"

namespace lisparser {
namespace macro {

using ArgumentMap = std::unordered_map<std::string, size_t>;

enum MacroError {
  INVALID_MACRO_FORM = 1,
};

struct Macro {
  Macro(ArgumentMap &&input_argument_id,
        std::vector<AST> &&input_bodies) :
      argument_id(std::move(input_argument_id)),
      bodies(std::move(input_bodies)) {}
  
  ArgumentMap argument_id;
  std::vector<AST> bodies;
};

class Engine {
 public:
  Engine() : _macros() {}

  util::Result<bool> Acquire(AST &&macro_ast);

  inline size_t size() const {
    return _macros.size();
  }
  
 private:
  std::unordered_map<std::string, Macro> _macros;
};


}  // namespace macro
}  // namespace lisparser
