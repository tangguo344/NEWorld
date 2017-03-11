cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_link_libraries(launcher nwshared)
