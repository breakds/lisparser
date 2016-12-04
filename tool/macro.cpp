#include <functional>
#include <utility>
#include "tool/macro.h"

namespace lisparser {
namespace macro {

namespace {
util::Result<ArgumentMap> AcquireArgumentMap(AST &&args) {
  if (args.type() != AST::LIST) {
    return util::Result<ArgumentMap>(
        INVALID_MACRO_FORM,
        "macro arguments should ba list");
  }

  std::vector<AST> arg_list = args.ReleaseVector();
  ArgumentMap argument_id;
  size_t current_id = 0;
  for (const AST &arg : arg_list) {
    if (arg.type() != AST::SYMBOL) {
      return util::Result<ArgumentMap>(
        INVALID_MACRO_FORM,
        "macro arguments should be all symbols");
    }

    if (argument_id.count(arg.AsString()) > 0) {
      return util::Result<ArgumentMap>(
        INVALID_MACRO_FORM,
        "duplicate argument in macro");
    }

    argument_id[arg.AsString()] = current_id;
    ++current_id;
  }

  return std::move(argument_id);
}

util::Result<bool> CheckBody(const ArgumentMap &argument_id,
                             const AST &body) {
  if (body.type() == AST::EVAL_FORM) {
    if (argument_id.count(body.AsString()) == 0) {
      return util::Result<bool>(
          INVALID_MACRO_FORM, "argument (" +
          body.AsString() + ") not in the lambda list");
    }
  } else if (body.type() == AST::LIST) {
    for (const AST &sub : body.AsVector()) {
      auto result = CheckBody(argument_id, sub);
      if (!result.ok()) {
        return result;
      }
    }
  }

  return true;
}
}  // namespace

util::Result<bool> Engine::Acquire(AST &&macro_ast) {
  if (macro_ast.type() != AST::LIST) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                        "macro defintion should be in list form");
  }

  if (!(macro_ast.car() == AST::Symbol("defmacro"))) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                        "macro defintion should start with 'defmacro'");
  }
  
  std::vector<AST> form = macro_ast.ReleaseVector();

  if (form.size() < 2) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                        "defmacro without macro name");
  }

  if (form[1].type() != AST::KEYWORD) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                        "macro name should be a keyword");
  }

  std::string name = form[1].AsString();
  auto error_message = [&name](const std::string &message) {
    return message + " in macro [" + name + "]";
  };
  

  if (form.size() < 4) {
    return util::Result<bool>(
        INVALID_MACRO_FORM,
        error_message("macro name should be a keyword"));
  }

  auto argument_id_result = AcquireArgumentMap(std::move(form[2]));
  if (!argument_id_result.ok()) {
    return util::Result<bool>(
        INVALID_MACRO_FORM,
        error_message(argument_id_result.error_message()));
  }
  ArgumentMap argument_id = *argument_id_result.value();

  std::vector<AST> bodies;
  for (size_t i = 3; i < form.size(); ++i) {
    auto result = CheckBody(argument_id, form[i]);
    if (!result.ok()) {
      return util::Result<bool>(
          INVALID_MACRO_FORM,
          error_message(result.error_message()));
    }
    bodies.push_back(std::move(form[i]));
  }

  _macros.emplace(std::piecewise_construct,
                  std::forward_as_tuple("name"),
                  std::forward_as_tuple(
                      std::move(argument_id), std::move(bodies)));

  return true;
}

}  // namespace macro
}  // namespace lisparser
