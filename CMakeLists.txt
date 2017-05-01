cmake_minimum_required(VERSION 3.2.2)   # CMake version check
project(Scramble_test)                  # Create project "simple_example"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # Enable c++11 standard

set(Boost_INCLUDE_DIR c:/mingw/include/)
set(Boost_LIBRARY_DIR c:/mingw/lib/)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)                      # Add main.cpp file of project root directory as source file
add_executable(Scramble_test ${SOURCE_FILES})   # Add executable target with source files listed in SOURCE_FILES variable

