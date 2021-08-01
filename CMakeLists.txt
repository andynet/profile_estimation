cmake_minimum_required(VERSION 3.16)
project(profile_estimation)
# set(CMAKE_C_STANDARD 99)

# message("CMAKE_CXX20_STANDARD_COMPILE_OPTION is " ${CMAKE_CXX20_STANDARD_COMPILE_OPTION} )
# message("standard is  " ${CMAKE_C_STANDARD})
link_directories(/usr/local/lib)
add_library(hashing hashing/src/hashing.h hashing/src/hashing.c)
add_library(safe_alloc hashing/src/safe_alloc.h hashing/src/safe_alloc.c)

add_executable(profile_estimation src/profile_estimation.c)
target_link_libraries(profile_estimation hts)

add_executable(test_functions test/test_functions.c)
target_link_libraries(test_functions criterion hashing safe_alloc yaml)

add_executable(run test/run.c)
target_link_libraries(run hashing safe_alloc yaml)