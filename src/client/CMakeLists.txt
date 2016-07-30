cmake_minimum_required(VERSION 2.8)

project(Client)
add_definitions(-DSDL_MAIN_HANDLED)

aux_source_directory(${SOURCE_DIR}/client SRC_CLIENT)
if(EXECUTABLE)
    add_executable(NEWorld ${SRC_CLIENT})
else()
    add_library(NEWorld SHARED ${SRC_CLIENT})
endif()
target_link_libraries(NEWorld pthread nwshared uilib freetype)
target_include_directories(NEWorld PUBLIC ${SOURCE_DIR}/shared PUBLIC)

