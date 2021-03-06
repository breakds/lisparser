cmake_minimum_required(VERSION 3.1)
project(code_gen)

message(STATUS "[Project Name]   ${PROJECT_NAME}")
message(STATUS "[Source Dir]     ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[Build Type]     ${CMAKE_BUILD_TYPE}")
message(STATUS "[Compiler]       ${CMAKE_CXX_COMPILER}")
message(STATUS "----------------------------------------------------------")

#############################################
## Project Setup and External Dependencies ##
#############################################

set(CMAKE_CXX_STANDARD 11)

# NOTE(breakds): if you are installing GTest on Ubuntu or Debian,
# libgtest-dev only installs the source at /usr/src/googletest. You
# need to build and install it with CMake by yourself.
find_package(GTest REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# TODO(breakds): Add prefix to all the targets.
add_library(lisparser_tokenizer tokenizer.cpp token.cpp)

add_library(lisparser_ast ast.cpp)
target_link_libraries(lisparser_ast lisparser_tokenizer)

add_library(lisparser parser.cpp)
target_link_libraries(lisparser lisparser_ast lisparser_tokenizer)

add_library(lisparser_macro tool/macro.cpp)
target_link_libraries(lisparser_macro
  lisparser lisparser_ast lisparser_tokenizer)

enable_testing()

add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test
  GTest::GTest GTest::Main
  lisparser_tokenizer)
GTEST_ADD_TESTS(tokenizer_test "" AUTO)

add_executable(ast_test ast_test.cpp)
target_link_libraries(ast_test
  GTest::GTest GTest::Main
  lisparser_ast)
GTEST_ADD_TESTS(ast_test "" AUTO)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test
  GTest::GTest GTest::Main
  lisparser)
GTEST_ADD_TESTS(parser_test "" AUTO)

add_executable(macro_test tool/macro_test.cpp)
target_link_libraries(macro_test
  GTest::GTest GTest::Main
  lisparser_macro)
GTEST_ADD_TESTS(macro_test "" AUTO)




