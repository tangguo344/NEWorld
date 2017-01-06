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
add_definitions()

aux_source_directory(${SOURCE_DIR}/client SRC_GAME)
aux_source_directory(${SOURCE_DIR}/client/imgui SRC_GAME)
aux_source_directory(${SOURCE_DIR}/client/renderer SRC_GAME)

aux_source_directory(${SOURCE_DIR}/game SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/api SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/world SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/plugin SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/network SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/context SRC_GAME)
aux_source_directory(${SOURCE_DIR}/game/renderer SRC_GAME)
aux_source_directory(${SOURCE_DIR}/engine/maintenance SRC_GAME)
add_library(v41 SHARED ${SRC_GAME})

target_include_directories(v41 PUBLIC ${SOURCE_DIR} PUBLIC)
target_include_directories(v41 PUBLIC ${SOURCE_DIR}/game PUBLIC)
SET_TARGET_PROPERTIES(v41 PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(v41 PROPERTIES SUFFIX "")
target_compile_definitions(v41 PRIVATE -DSDL_MAIN_HANDLED PRIVATE -DNWCOREEXPORTS PRIVATE -DNWCompartmentLoggerPrefix="nwcore")

target_link_libraries(v41 pthread freetype RakNetDLL SDL2 SDL2_image GLEW)

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    target_link_libraries(v41 "-framework OpenGL")
endif()

project(Launcher)
aux_source_directory(${SOURCE_DIR}/launcher SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_link_libraries(launcher v41)
target_compile_definitions(launcher PRIVATE -DNWCompartmentLoggerPrefix="launcher")

add_subdirectory(./plugins)


