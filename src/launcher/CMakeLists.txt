cmake_minimum_required(VERSION 2.8)

project(Launcher)
aux_source_directory(${SOURCE_DIR}/launcher SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_link_libraries(launcher nwshared)
target_include_directories(launcher PUBLIC ${SOURCE_DIR}/shared)

