# test cmake

# tests are profiled
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "-pg")
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary
    ${RANDOMWALK_SOURCE_DIR}/src/RandomWalk)

set(RANDOMWALK_LIBS Boundary RandomWalk Utils)

add_executable(RandomWalk_test main.cpp)
target_link_libraries(RandomWalk_test ${RANDOMWALK_LIBS})

add_executable(RandomWalkBenchmark benchmark.cpp)
target_link_libraries(RandomWalkBenchmark ${RANDOMWALK_LIBS})

add_executable(CellDensity_test CellDensityTest.cpp)
target_link_libraries(CellDensity_test ${RANDOMWALK_LIBS})

add_executable(TerminateTest TerminateTest.cpp)

