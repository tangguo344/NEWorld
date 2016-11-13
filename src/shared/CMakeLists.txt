project(Shared)
aux_source_directory(${SOURCE_DIR}/shared SRC_SHARED)
add_library(nwshared STATIC ${SRC_SHARED})
target_include_directories(nwshared PUBLIC ${SOURCE_DIR}/shared)