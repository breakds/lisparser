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

add_library(tokenizer tokenizer.cpp token.cpp)

enable_testing()

add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test
  GTest::GTest GTest::Main
  tokenizer)
GTEST_ADD_TESTS(tokenizer_test "" AUTO)


