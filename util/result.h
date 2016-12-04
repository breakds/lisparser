#pragma once

#include <memory>
#include <string>

namespace lisparser {
namespace util {

template <typename ValueType>
class Result {
 public:
  enum {
    OK = 0,
  };
  
  Result(ValueType &&value)
      : _value(new ValueType(std::move(value))),
        _error_code(OK), _error_message() {}

  Result(int error_code)
      : _value(nullptr), _error_code(error_code) {}

  Result(int error_code, std::string &&error_message) 
      : _value(nullptr),
        _error_code(error_code),
        _error_message(error_message) {}

  Result(Result<ValueType> &&other)
      : _value(std::move(other._value)),
        _error_code(other._error_code),
        _error_message(std::move(other._error_message)) {}

  Result<ValueType> &operator=(Result<ValueType> &&other) {
    _value = std::move(other._value);
    _error_code = other._error_code;
    _error_message.swap(other._error_message);
    return *this;
  }


  inline bool ok() const {
    return _error_code == OK;
  }

  std::unique_ptr<ValueType> value() {
    assert(_error_code == OK);
    return std::move(_value);
  }

  inline int error_code() const {
    return _error_code;
  }

  inline const std::string &error_message() const {
    return error_message;
  }
  
 private:
  Result(const Result<ValueType> &other) = delete;
  Result<ValueType> &operator=(const Result &other) = delete;
  
  std::unique_ptr<ValueType> _value;
  int _error_code;
  std::string _error_message;
};

}  // namespace util
}  // namespace lisparser
