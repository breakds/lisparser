// TODO(breakds): Support multi-object-return macro.
// TODO(breakds): Support macro dependency check.

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
  INVALID_MACRO_ARGS = 1,
  INVALID_MACRO_FORM = 2,
};

enum MacroEvaluateError {
  SIGNATURE_MISMATCH = 1,
};

struct Macro {
  Macro(ArgumentMap &&input_argument_id,
        AST &&input_body) :
      argument_id(std::move(input_argument_id)),
      body(std::move(input_body)) {}
  
  ArgumentMap argument_id;
  AST body;
};

class Engine {
 public:
  Engine() : _macros() {}

  util::Result<bool> Acquire(AST &&macro_ast);

  util::Result<AST> Evaluate(const AST &original);

  inline size_t size() const {
    return _macros.size();
  }
  
 private:
  AST Expand(const AST &body,
             const ArgumentMap &argument_id,
             const AST &macro_form);

  std::unordered_map<std::string, Macro> _macros;
};

}  // namespace macro
}  // namespace lisparser
