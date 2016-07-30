cmake_minimum_required(VERSION 2.8)

project(Shared)
aux_source_directory(${SOURCE_DIR}/shared SRC_SHARED)
add_library(nwshared STATIC ${SRC_SHARED})

