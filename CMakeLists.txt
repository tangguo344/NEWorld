cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_DIR ./src)

option(EXECUTABLE "Link the executable for client and server" OFF)

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

include(${SOURCE_DIR}/shared/CMakeLists.txt)
include(${SOURCE_DIR}/client/CMakeLists.txt)
include(${SOURCE_DIR}/server/CMakeLists.txt)
include(${SOURCE_DIR}/launcher/CMakeLists.txt)
include(${SOURCE_DIR}/main/CMakeLists.txt)

add_dependencies(nwclient nwshared)
add_dependencies(nwserver nwshared)
add_dependencies(launcher nwclient)
add_dependencies(launcher nwserver)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()
