cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_DIR ./../../src)

project(MainPlugin)

aux_source_directory(${SOURCE_DIR}/main/client SRC_MAINPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/shared SRC_MAINPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/server SRC_MAINSERVERPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/shared SRC_MAINSERVERPLUGIN)
add_library(main SHARED ${SRC_MAINPLUGIN})
add_library(mainserver SHARED ${SRC_MAINSERVERPLUGIN})
target_link_libraries(main nwclient nwcore)
target_link_libraries(mainserver nwserver nwcore)

SET_TARGET_PROPERTIES(main mainserver PROPERTIES PREFIX "")
