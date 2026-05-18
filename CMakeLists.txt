cmake_minimum_required(VERSION 3.25)
project(gram0)

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_library(gram0lib STATIC lib/gram0.cpp)
target_include_directories(gram0lib PUBLIC lib)

add_executable(gram0 main.cpp)
set_target_properties(gram0 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(gram0 PRIVATE gram0lib)

enable_testing()
add_subdirectory(tests)
