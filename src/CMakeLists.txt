cmake_minimum_required(VERSION 3.1)

# Set search paths
set(DEP_INC_PATH "" CACHE PATH "The path to depended includes")
set(DEP_LIB_PATH "" CACHE PATH "The path to depended libraries")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${DEP_INC_PATH} " " ${DEP_LIB_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_ROOT_PATH}/cmake)

# Set build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(UNIX)
    link_libraries(dl)
endif()

if(MS_VC)
    add_definitions(-DNOMINMAX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

Project(NEWorld)

# public incldue directory
include_directories(common)

add_subdirectory(common)
add_subdirectory(neworld)
add_subdirectory(neworldlauncher)

add_dependencies(nwclient nwshared)
add_dependencies(launcher nwclient)
