#pragma once

#include <cctype>

namespace lisparser {
namespace util {
namespace char_ops {

inline bool Skipper(int character) {
  // In the default locale, the whitespace characters are the
  // following:
  //
  // space (0x20, ' ')
  // form feed (0x0c, '\f')
  // line feed (0x0a, '\n')
  // carriage return (0x0d, '\r')
  // horizontal tab (0x09, '\t')
  // vertical tab (0x0b, '\v')

  return std::isspace(character);
}

inline bool SymbolCharacter(int character) {
  return (std::isalpha(character) ||
          std::isdigit(character) ||
          (std::ispunct(character) &&
           static_cast<char>(character) != '(' &&
           static_cast<char>(character) != ')' &&
           static_cast<char>(character) != ',' &&
           static_cast<char>(character) != '\'' &&
           static_cast<char>(character) != '"'));
}

}  // namespace char_ops
}  // namespace util
}  // namespace lisparser
