cmake_minimum_required(VERSION 3.9)
project(ter C)

link_libraries("/usr/lib/x86_64-linux-gnu/libcurses.so")

set(CMAKE_C_STANDARD 11)

add_executable(ter boom/client.c boom/multialarm.c boom/multialarm.h boom/multialarmtest.c)
#add_executable(ter test.c)
