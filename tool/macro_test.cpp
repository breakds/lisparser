#include "tool/macro.h"

#include "gtest/gtest.h"

namespace lisparser {
namespace macro {

TEST(Macro, EngineSize) {
  Engine engine;

  engine.Acquire(AST::Vector(
      AST::Symbol("defmacro"),
      AST::Keyword(":something"),
      AST::Vector(
          AST::Symbol("a"),
          AST::Symbol("b")),
      AST::Vector(
          AST::Symbol("+"),
          AST::EvalForm("a"),
          AST::EvalForm("b"))));

  EXPECT_EQ(1, engine.size());
}

}  // namespace macro
}  // namespace lisparser


