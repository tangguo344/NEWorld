cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

add_definitions(-DNEWORLD_CLIENT)
# Link OpenGL & GLEW
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
# Link SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
# Link Freetype
find_package(Freetype REQUIRED)

set(DEPEND_LIBS_INC ${DEPEND_LIBS_INC} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
set(DEPEND_LIBS ${DEPEND_LIBS} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY}
    ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${FREETYPE_LIBRARIES})

aux_source_directory(. SRC_CLIENT)
aux_source_directory(imgui SRC_CLIENT)

# Set Dependency Include Dirs
include_directories(PUBLIC . ${DEPEND_LIBS_INC})

add_library(nwclient SHARED ${SRC_CLIENT})
target_link_libraries(nwclient pthread nwshared RakNetDLL ${DEPEND_LIBS})
