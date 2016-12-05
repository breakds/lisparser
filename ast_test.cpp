#include "ast.h"

#include "gtest/gtest.h"

namespace lisparser {

TEST(AST, CopyTest) {
  AST ast = AST::Vector(
      AST::Keyword(":abc"),
      AST::Vector(
          AST::Integer(3115),
          AST::Vector(
              AST::Symbol("xyz"),
              AST::Vector()),
          AST::Double(4.18),
          AST::EvalForm("some-variable")),
      AST::Vector());
  
  EXPECT_EQ(ast, ast.Copy());
}
}  // namespace lisparser
