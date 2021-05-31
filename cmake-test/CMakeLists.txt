cmake_minimum_required(VERSION 3.10)

# Also note how we set the CMAKE_CXX_STANDARD variable. This will allow us to use modern C++ features (available in C++ 11 or later) such as the auto keyword or raw string literals later on.

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(HEADER_FILE_PATH "${PROJECT_DIR}/include/")
set(SRC_FILE_PATH "${PROJECT_DIR}/src/")

project(hello)

# The add_library and add_executable commands describe the two targets (named # hello and hello_demo) and the sources used to build them, and the 
# target_link_libraries command tells CMake that hello_demo depends on 
# hello.

add_library(
  hello
  ${HEADER_FILE_PATH}/hello.hpp
  ${SRC_FILE_PATH}/hello.cpp
)

# The target_include_directories informs CMake that there are header files in the include directories that should be used both when builing the library itself, but also by consumers of the library. (We used the -I include/ flag both for building libchucknorris.o and main.co)

target_include_directories(
  hello
  PUBLIC
  "include"
)

add_executable(
  hello_demo
  ${SRC_FILE_PATH}/main.cpp  
)

target_link_libraries(hello_demo hello)