cmake_minimum_required(VERSION 3.10)
project(RegexToMinDFA)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Core source files
add_library(nfa STATIC src/nfa.cpp)
add_library(dfa STATIC src/dfa.cpp)

# Main executable
add_executable(main src/main.cpp)
target_link_libraries(main nfa dfa)

# Test executable
add_executable(test_all test/test_all.cpp)
target_link_libraries(test_all nfa dfa)
