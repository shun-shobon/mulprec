project(mulprec LANGUAGES C)

add_library(util util.c)
add_library(fft fft.c)
add_library(mulprec mulprec.c)

add_executable(main main.c)
target_link_libraries(main mulprec fft util)
