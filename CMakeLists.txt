cmake_minimum_required(VERSION 3.27)
project(Battleship LANGUAGES CXX)

include_directories("libs")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)