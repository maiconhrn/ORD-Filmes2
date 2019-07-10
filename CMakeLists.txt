cmake_minimum_required(VERSION 3.14)
project(Trabalho2 C)

set(CMAKE_C_STANDARD 99)

add_executable(Trabalho2 btree.h main.c commands.h commands.c bool.h btree.c keyoffset.h keyoffset.c)