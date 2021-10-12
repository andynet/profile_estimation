cmake_minimum_required(VERSION 3.16)
project(profile_estimation)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -march=native -O3")

link_directories(/usr/local/lib)
add_library(hashing hashing/src/hashing.h hashing/src/hashing.c)
add_library(safe_alloc hashing/src/safe_alloc.h hashing/src/safe_alloc.c)
add_library(ndarray src/ndarray.h src/ndarray.c)

add_executable(ndarray_test test/test_ndarray.c)
target_link_libraries(ndarray_test ndarray criterion)

add_executable(test_functions test/test_functions.c)
target_link_libraries(test_functions criterion ndarray hashing safe_alloc yaml hts m)

add_executable(run test/run.c)
target_link_libraries(run ndarray hashing safe_alloc yaml hts m)

add_executable(profile_estimation src/profile_estimation.c)
target_link_libraries(profile_estimation ndarray hashing safe_alloc yaml hts m)

add_executable(timed_profile_estimation src/timed_profile_estimation.c)
target_link_libraries(timed_profile_estimation ndarray hashing safe_alloc yaml hts m)
