cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. SRC_SHARED)
add_library(nwshared STATIC ${SRC_SHARED})
