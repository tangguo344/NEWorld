cmake_minimum_required(VERSION 2.8)

project(UILib)
aux_source_directory(${SOURCE_DIR}/uilib/src SRC_UILIB)
add_library(uilib STATIC ${SRC_UILIB})
target_link_libraries(uilib GL GLEW SDL2 SDL2main SDL2_image)
target_include_directories(uilib PUBLIC ${SOURCE_DIR}/uilib/include)

