cmake_minimum_required(VERSION 3.30)
project(P3_25 C)

set(CMAKE_C_STANDARD 11)

add_executable(P3_25
            factory_manager.c
            process_manager.c
            queue.c
            queue.h)