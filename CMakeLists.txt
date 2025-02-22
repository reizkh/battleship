cmake_minimum_required(VERSION 3.27)
project(Battleship LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("${CMAKE_SOURCE_DIR}/lib" "${CMAKE_SOURCE_DIR}/include")
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(src)
