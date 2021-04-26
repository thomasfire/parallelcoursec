cmake_minimum_required(VERSION 3.17)
project(lab_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab_1 main.c)
target_link_libraries(lab_1 m)