project(Server)
add_definitions(-DNEWORLD_SERVER)

aux_source_directory(${SOURCE_DIR}/server SRC_SERVER)
if(EXECUTABLE)
    add_executable(nwserver ${SRC_SERVER})
else()
    add_library(nwserver SHARED ${SRC_SERVER})
endif()
target_link_libraries(nwserver nwcore nwshared pthread RakNetDLL)
target_include_directories(nwserver PUBLIC ${SOURCE_DIR}/shared)
target_include_directories(nwserver PUBLIC ${SOURCE_DIR}/nwcore PUBLIC)

