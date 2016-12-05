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
  size_t current_id = 1;
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

  if (form.size() != 4) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                              "defmacro form should be of length 4.");
  }

  if (form[1].type() != AST::KEYWORD) {
    return util::Result<bool>(INVALID_MACRO_FORM,
                        "macro name should be a keyword");
  }

  std::string name = form[1].AsString();
  auto error_message = [&name](const std::string &message) {
    return message + " in macro [" + name + "]";
  };
  
  auto argument_id_result = AcquireArgumentMap(std::move(form[2]));
  if (!argument_id_result.ok()) {
    return util::Result<bool>(
        INVALID_MACRO_FORM,
        error_message(argument_id_result.error_message()));
  }
  ArgumentMap argument_id = *argument_id_result.value();

  auto result = CheckBody(argument_id, form[3]);
  if (!result.ok()) {
    return util::Result<bool>(
        INVALID_MACRO_FORM,
        error_message(result.error_message()));
  }
  
  _macros.emplace(std::piecewise_construct,
                  std::forward_as_tuple(name),
                  std::forward_as_tuple(
                      std::move(argument_id), std::move(form[3])));

  return true;
}

util::Result<AST> Engine::Evaluate(const AST &original) {
  if (original.type() == AST::LIST) {
    AST new_form = AST::Vector();
    for (const AST &element : original.AsVector()) {
      auto evaluated = Evaluate(element);

      if (!evaluated.ok()) {
        return evaluated;
      }
      
      new_form.Push(std::move(*evaluated.value()));
    }

    if (new_form.car().type() == AST::KEYWORD) {
      auto macro = _macros.find(new_form.car().AsString());
      if (macro != _macros.end()) {
        if (macro->second.argument_id.size() + 1 !=
            new_form.AsVector().size()) {
          return util::Result<AST>(
              SIGNATURE_MISMATCH,
              util::StrCat(macro->first, " wanted ",
                           macro->second.argument_id.size(),
                           " arguments, but only ",
                           new_form.AsVector().size() - 1,
                           " are provided."));
        }
            
        auto result = Evaluate(Expand(macro->second.body,
                                      macro->second.argument_id,
                                      new_form));
        if (!result.ok()) return result;
        return result;
      }
    }
    
    return std::move(new_form);
  } 

  return original.Copy();
}


AST Engine::Expand(const AST &body,
                   const ArgumentMap &argument_id,
                   const AST &macro_form) {
  if (body.type() == AST::EVAL_FORM) {
    auto iter = argument_id.find(body.AsString());
    assert(iter != argument_id.end());
    assert(macro_form.AsVector().size() > iter->second);
    return macro_form.AsVector()[iter->second].Copy();
  } else if (body.type() == AST::LIST) {
    AST result = AST::Vector();
    for (const AST &sub : body.AsVector()) {
      result.Push(Expand(sub, argument_id, macro_form));
    }
    return result;
  }
  
  return body.Copy();
}

}  // namespace macro
}  // namespace lisparser
