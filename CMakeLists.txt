cmake_minimum_required(VERSION 3.0)

project(libpstretch C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# makes tests work
include_directories(lib/include)

add_subdirectory(lib)
add_subdirectory(main)

enable_testing()
add_subdirectory(tests)
