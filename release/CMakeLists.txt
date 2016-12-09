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

aux_source_directory(${SOURCE_DIR}/client SRC_CLIENT)
aux_source_directory(${SOURCE_DIR}/client/imgui SRC_CLIENT)
aux_source_directory(${SOURCE_DIR}/server SRC_SERVER)
aux_source_directory(${SOURCE_DIR}/nwcore SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/api SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/world SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/plugin SRC_CORE)
aux_source_directory(${SOURCE_DIR}/nwcore/common SRC_CORE)

add_library(nwserver SHARED ${SRC_SERVER})
add_library(nwclient SHARED ${SRC_CLIENT})
add_library(nwshared STATIC ${SRC_SHARED})
add_library(nwcore SHARED ${SRC_CORE})

target_include_directories(nwclient PUBLIC ${SOURCE_DIR}/shared PUBLIC)
target_include_directories(nwclient PUBLIC ${SOURCE_DIR}/nwcore PUBLIC)
target_include_directories(nwserver PUBLIC ${SOURCE_DIR}/shared)
target_include_directories(nwserver PUBLIC ${SOURCE_DIR}/nwcore PUBLIC)
target_include_directories(nwcore PUBLIC ${SOURCE_DIR}/nwcore PUBLIC)
target_include_directories(nwcore PUBLIC ${SOURCE_DIR}/shared PUBLIC)

SET_TARGET_PROPERTIES(nwclient nwserver PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(nwclient nwserver PROPERTIES SUFFIX "")


target_link_libraries(nwserver nwcore pthread RakNetDLL)
target_link_libraries(nwclient nwcore pthread freetype RakNetDLL SDL2 SDL2_image GLEW)
if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    target_link_libraries(nwclient "-framework OpenGL")
endif()

project(Launcher)
aux_source_directory(${SOURCE_DIR}/launcher SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_link_libraries(launcher nwcore)
target_include_directories(launcher PUBLIC ${SOURCE_DIR}/shared)

add_dependencies(launcher nwclient)
add_dependencies(launcher nwserver)
add_dependencies(launcher nwcore)

add_subdirectory(./plugins)


