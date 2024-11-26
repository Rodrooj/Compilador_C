cmake_minimum_required(VERSION 3.29)
project(compilador C)

set(CMAKE_C_STANDARD 11)

add_executable(compilador
        main_old.c
        hashTable_old.c
        tokens.h
        main.c
        main_not-working.c
        symbol_table.h
        parser.h
        parser.c
        symbol_table.c)
