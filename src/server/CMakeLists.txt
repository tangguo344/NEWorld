cmake_minimum_required(VERSION 2.8)

project(Server)
aux_source_directory(${SOURCE_DIR}/server SRC_SERVER)
if(EXECUTABLE)
    add_executable(NEWorldServer ${SRC_SERVER})
else()
    add_library(NEWorldServer SHARED ${SRC_SERVER})
endif()
target_link_libraries(NEWorldServer nwshared pthread)
target_include_directories(NEWorldServer PUBLIC ${SOURCE_DIR}/shared)

