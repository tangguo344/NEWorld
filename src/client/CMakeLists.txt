project(Client)
add_definitions(-DSDL_MAIN_HANDLED)
add_definitions(-DNEWORLD_CLIENT)

aux_source_directory(${SOURCE_DIR}/client SRC_CLIENT)
aux_source_directory(${SOURCE_DIR}/client/imgui SRC_CLIENT)

if(EXECUTABLE)
    add_executable(nwclient ${SRC_CLIENT})
else()
    add_library(nwclient SHARED ${SRC_CLIENT})
endif()
target_link_libraries(nwclient pthread nwshared freetype RakNetDLL SDL2 SDL2_image glew )

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    target_link_libraries(nwclient "-framework OpenGL")
endif()

target_include_directories(nwclient PUBLIC ${SOURCE_DIR}/shared PUBLIC)
