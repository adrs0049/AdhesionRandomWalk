# Simulator cmake

find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary)

set(RANDOMWALK_SOURCES Simulator.cpp RandomWalk.cpp)
add_library(RandomWalk SHARED ${RANDOMWALK_SOURCES})

# now build the python interface

set(SIMULATOR_SWIG_SOURCES simulator.i)

set(CMAKE_SWIG_FLAGS "-Wall")
include(${SWIG_USE_FILE})
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SWIG_USE_FILE}
    ${PYTHON_INCLUDE_PATH}
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary)

set_source_files_properties(${SIMULATOR_SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(simulator python ${SIMULATOR_SWIG_SOURCES})
swig_link_libraries(simulator RandomWalk Boundary ${PYTHON_LIBRARIES})
