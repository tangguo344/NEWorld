cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_DIR ./../src)

if(EXTRA_INCDIR)
    include_directories(BEFORE ${EXTRA_INCDIR})
endif()

if(EXTRA_LIBDIR)
    link_directories(${EXTRA_LIBDIR})
endif()

if(UNIX)
    link_libraries(dl)
endif()

if(MS_VC)
    add_definitions(-DNOMINMAX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

project(NEWorld)
add_definitions(-DSDL_MAIN_HANDLED)
add_definitions(-DNWCOREEXPORTS)

aux_source_directory(${SOURCE_DIR}/client SRC_CORE)
aux_source_directory(${SOURCE_DIR}/client/imgui SRC_CORE)
aux_source_directory(${SOURCE_DIR}/server SRC_CORE)

aux_source_directory(${SOURCE_DIR}/nwcore SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/api SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/world SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/plugin SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/common/maintenance SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/context SRC_CORE)

add_library(nwcore SHARED ${SRC_CORE})

target_include_directories(nwcore PUBLIC ${SOURCE_DIR}/nwcore PUBLIC)
SET_TARGET_PROPERTIES(nwcore PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(nwcore PROPERTIES SUFFIX "")

target_link_libraries(nwcore pthread freetype RakNetDLL SDL2 SDL2_image GLEW)

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    target_link_libraries(nwcore "-framework OpenGL")
endif()

project(Launcher)
aux_source_directory(${SOURCE_DIR}/launcher SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_link_libraries(launcher nwcore)
add_dependencies(launcher nwcore)

add_subdirectory(./plugins)


