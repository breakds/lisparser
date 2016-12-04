#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace lisparser {

namespace internal {
template <typename AnyType>
void DefaultDelete(void *target) {
  std::default_delete<AnyType>()(reinterpret_cast<AnyType*>(target));
}
}  // namespace internal

class AST {
 public:
  using ValuePointer = std::unique_ptr<void, std::function<void(void*)>>;

  enum Type {
    LIST = 100,
    EVAL_FORM = 101,
    KEYWORD = 0,
    SYMBOL = 1,
    STRING = 2,
    FLOAT = 3,
    INTEGER = 4,
  };

  AST(Type type, ValuePointer &&value)
      : _type(type), _value(std::move(value)) {}

  AST(AST &&other)
      : _type(other._type), _value(std::move(other._value)) {}

  AST &operator=(AST &&other) {
    _type = other._type;
    _value = std::move(other._value);
    return *this;
  }

  static AST Keyword(std::string &&name);
  static AST Symbol(std::string &&name);
  static AST String(std::string &&content);
  static AST EvalForm(std::string &&variable);
  static AST Integer(int64_t value);
  static AST Double(double value);
  static AST Vector();

  bool operator==(const AST&other) const;
  
  inline Type type() const {
    return _type;
  }

  void Push(AST &&element) {
    assert(_type == LIST);
    reinterpret_cast<std::vector<AST>*>(_value.get())->push_back(
        std::move(element));
  }
  
  const std::string &AsString() const {
    return *reinterpret_cast<std::string*>(_value.get());
  }

  double AsDouble() const {
    return *reinterpret_cast<double*>(_value.get());
  }

  int64_t AsInt64() const {
    return *reinterpret_cast<int64_t*>(_value.get());
  }

  const std::vector<AST> &AsVector() const {
    return *reinterpret_cast<std::vector<AST>*>(_value.get());
  }

 private:
  AST(const AST &other) = delete;
  AST &operator=(const AST &other) = delete;

  Type _type;
  ValuePointer _value;
};

std::ostream &operator<<(std::ostream &output, const AST &ast);

}  // namespace lisparser