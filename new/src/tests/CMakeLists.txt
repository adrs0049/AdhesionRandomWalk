# test cmake

# tests are profiled
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "-pg")
endif(CMAKE_COMPILER_IS_GNUCC)

find_package (Boost)
if (Boost_FOUND)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost COMPONENTS  unit_test_framework REQUIRED)
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

include_directories(
    "${Boost_INCLUDE_DIRS}"
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
    ${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary
    ${RANDOMWALK_SOURCE_DIR}/src/RandomWalk)

add_executable(RandomWalk_test main.cpp)
target_link_libraries(RandomWalk_test Boundary ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Utils)

add_executable(RandomWalkBenchmark benchmark.cpp)
target_link_libraries(RandomWalkBenchmark Boundary Utils)

add_executable(CellDensity_test CellDensityTest.cpp)
target_link_libraries(CellDensity_test Boundary ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Utils)

