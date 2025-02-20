cmake_minimum_required(VERSION 3.30)
project(ssoo_p1_intial_code C)

set(CMAKE_C_STANDARD 11)

add_executable(
        ssoo_p1_initial_code
        crear.c
        autores.txt
        combine.c
)

add_custom_command(TARGET ssoo_p1_initial_code POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E chmod 777 $<TARGET_FILE:ssoo_p1_initial_code>
)