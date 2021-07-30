cmake_minimum_required(VERSION 3.16)
project(profile_estimation)

link_directories(/usr/local/lib)

add_executable(profile_estimation src/profile_estimation.c)
target_link_libraries(profile_estimation hts)

add_executable(test_functions test/test_functions.c)
target_link_libraries(test_functions criterion)

add_executable(run test/run.c)