cmake_minimum_required(VERSION 3.1...3.22)
project(mulprec LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
