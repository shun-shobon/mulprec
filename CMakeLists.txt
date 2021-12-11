cmake_minimum_required(VERSION 3.1...3.22)
project(mulprec LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -pipe -O3 -flto -mtune=native -march=native")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
