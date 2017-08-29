cmake_minimum_required(VERSION 3.1)
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 14)

# Set build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Internal/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product)

# Set Platform Specific Builder Flags
if(UNIX)
    link_libraries(dl)
endif()

if(MS_VC)
    add_definitions(-DNOMINMAX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# Copy Resorces
file(COPY ${PROJECT_ROOT_PATH}/Resource DESTINATION ${CMAKE_BINARY_DIR}/Product)

# Build Project
add_subdirectory(${PROJECT_ROOT_PATH}/Src Internal)
