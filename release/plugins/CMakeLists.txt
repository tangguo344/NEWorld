cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_DIR ./../../src)

project(MainPlugin)

aux_source_directory(${SOURCE_DIR}/main/server SRC_MAINPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/shared SRC_MAINPLUGIN)
add_library(main SHARED ${SRC_MAINPLUGIN})
target_link_libraries(main nwcore)

SET_TARGET_PROPERTIES(main PROPERTIES PREFIX "")
