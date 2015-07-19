# Simulator cmake

set(Python_ADDITIONAL_VERSIONS 3.3)

find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(NumPy REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
	${RANDOMWALK_SOURCE_DIR}/utils/vector_simd
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary)

set(RANDOMWALK_SOURCES Parameters.cpp Simulator.cpp RandomWalk.cpp
	EventListener.cpp Propensities.cpp)
add_library(RandomWalk SHARED ${RANDOMWALK_SOURCES})
target_link_libraries(RandomWalk Utils)

# now build the python interface

set(SIMULATOR_SWIG_SOURCES simulator.i)

set(CMAKE_SWIG_FLAGS "-Wall")
include(${SWIG_USE_FILE})
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SWIG_USE_FILE}
    ${PYTHON_INCLUDE_PATH}
    ${NUMPY_INCLUDE_DIRS}
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary)

set_source_files_properties(${SIMULATOR_SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)
set_source_files_properties(${SIMULATOR_SWIG_SOURCES} PROPERTIES SWIG_FLAGS "-py3")
set(CMAKE_SWIG_OUTDIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

SWIG_ADD_MODULE(simulator python ${SIMULATOR_SWIG_SOURCES})
swig_link_libraries(simulator RandomWalk Boundary ${PYTHON_LIBRARIES})

set_source_files_properties(${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "-O2")

