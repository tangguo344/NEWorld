cmake_minimum_required(VERSION 2.8)

project(Server)
aux_source_directory(${SOURCE_DIR}/server SRC_SERVER)
if(EXECUTABLE)
    add_executable(nwserver ${SRC_SERVER})
else()
    add_library(nwserver SHARED ${SRC_SERVER})
endif()
target_link_libraries(nwserver nwshared pthread)
target_include_directories(nwserver PUBLIC ${SOURCE_DIR}/shared)

