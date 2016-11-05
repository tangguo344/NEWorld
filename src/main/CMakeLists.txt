cmake_minimum_required(VERSION 2.8)

project(MainPlugin)

aux_source_directory(${SOURCE_DIR}/main/client SRC_MAINPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/shared SRC_MAINPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/server SRC_MAINSERVERPLUGIN)
aux_source_directory(${SOURCE_DIR}/main/shared SRC_MAINSERVERPLUGIN)
add_library(main SHARED ${SRC_MAINPLUGIN})
add_library(mainserver SHARED ${SRC_MAINSERVERPLUGIN})
target_link_libraries(main nwclient)
target_link_libraries(mainserver nwserver)

SET_TARGET_PROPERTIES(main mainserver PROPERTIES PREFIX "")

add_dependencies(main nwclient)
add_dependencies(mainserver nwserver)

