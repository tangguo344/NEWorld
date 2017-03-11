cmake_minimum_required(VERSION 3.1)

aux_source_directory(client SRC_MAINPLUGIN)
aux_source_directory(shared SRC_MAINPLUGIN)
aux_source_directory(server SRC_MAINSERVERPLUGIN)
aux_source_directory(shared SRC_MAINSERVERPLUGIN)

add_library(main SHARED ${SRC_MAINPLUGIN})
add_library(mainserver SHARED ${SRC_MAINSERVERPLUGIN})

target_link_libraries(main nwclient)
target_link_libraries(mainserver nwserver)

SET_TARGET_PROPERTIES(main mainserver PROPERTIES PREFIX "")
