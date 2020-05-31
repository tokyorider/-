cmake_minimum_required(VERSION 3.10)
project(lab4)

find_package (Threads REQUIRED)

#target_link_libraries(lab4 ${CMAKE_THREAD_LIBS_INIT})


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2 -D_GNU_SOURCE  -fsanitize=address -pthread")


#add_executable(lab4 tests.h parallel_cluster.c tests_cluster.c)
add_executable(lab4 main.c tests/load.c tests/tests.h tests/store.c utils.c tests/store.c tests/copy.c)