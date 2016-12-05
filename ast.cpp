#include "ast.h"

#include <cmath>
#include "token.h"

namespace lisparser {

namespace {
constexpr double EPSILON_FOR_FLOAT_COMPARISON = 1e-6;
}

AST AST::Keyword(std::string &&name) {
  return AST(AST::KEYWORD,
             ValuePointer(new std::string(std::move(name)),
                          &internal::DefaultDelete<std::string>));
}

AST AST::Symbol(std::string &&name) {
  return AST(AST::SYMBOL,
             ValuePointer(new std::string(std::move(name)),
                          &internal::DefaultDelete<std::string>));
}

AST AST::String(std::string &&content) {
  return AST(AST::STRING,
             ValuePointer(new std::string(std::move(content)),
                          &internal::DefaultDelete<std::string>));
}

AST AST::EvalForm(std::string &&variable) {
  return AST(AST::EVAL_FORM,
             ValuePointer(new std::string(std::move(variable)),
                          &internal::DefaultDelete<std::string>));
}

AST AST::Integer(int64_t value) {
  return AST(AST::INTEGER,
             ValuePointer(new int64_t(value),
                          &internal::DefaultDelete<int64_t>));
}

AST AST::Double(double value) {
  return AST(AST::FLOAT,
             ValuePointer(new double(value),
                          &internal::DefaultDelete<double>));
}

bool AST::operator==(const AST&other) const {
  if (_type != other._type) return false;
  
  switch (_type) {
    case SYMBOL:
    case KEYWORD:
    case STRING:
    case EVAL_FORM:
      return AsString() == other.AsString();

    case FLOAT:
      return std::abs(AsDouble() - other.AsDouble())
          < EPSILON_FOR_FLOAT_COMPARISON;

    case INTEGER:
      return AsInt64() == other.AsInt64();

    case LIST:
      return AsVector() == other.AsVector();
  }

  // This is not necessary and is put here to please some werid
  // compilers (MSVC, I am staring at you).
  return false;
}

AST AST::Copy() const {
  switch (type()) {
    case AST::KEYWORD:
      return Keyword(std::string(AsString()));

    case AST::SYMBOL:
      return Symbol(std::string(AsString()));

    case AST::STRING:
      return String(std::string(AsString()));

    case AST::EVAL_FORM:
      return EvalForm(std::string(AsString()));

    case AST::INTEGER:
      return Integer(AsInt64());
      
    case AST::FLOAT:
      return Double(AsDouble());

    case AST::LIST: {
      AST result = Vector();
      for (const AST &element : AsVector()) {
        result.Push(element.Copy());
      }
      return result;
    }
  }
}

std::ostream &operator<<(std::ostream &output, const AST &ast) {
  switch (ast.type()) {
    case AST::KEYWORD:
    case AST::SYMBOL:
      output << ast.AsString();
      break;

    case AST::STRING:
      output << '"' << ast.AsString() << '"';
      break;

    case AST::EVAL_FORM:
      output << ',' << ast.AsString();
      break;

    case AST::INTEGER:
      output << ast.AsInt64();
      break;
      
    case AST::FLOAT:
      output << ast.AsDouble();
      break;

    case AST::LIST: {
      const std::vector<AST> &elements = ast.AsVector();
      output << '(';
      bool first = true;
      for (const AST &element : elements) {
        if (!first) output << ' ';
        output << element;
        first = false;
      }
      output << ')';
      break;
    }
  }
  
  return output;
}

}  // namespace lisparser
