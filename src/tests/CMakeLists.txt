# test cmake

# find cxxtest
find_package(PythonInterp REQUIRED)
find_package(CxxTest)

# tests are profiled
#if (CMAKE_COMPILER_IS_GNUCC)
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
#    set(CMAKE_EXE_LINKER_FLAGS "-pg")
#endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
	${RANDOMWALK_SOURCE_DIR}/utils/vector_simd
	${RANDOMWALK_SOURCE_DIR}/utils/spdlog
	${RANDOMWALK_SOURCE_DIR}/src/Random
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary
    ${RANDOMWALK_SOURCE_DIR}/src/RandomWalk)

set(RANDOMWALK_LIBS Boundary RandomWalk Utils)

add_executable(RandomWalk_test main.cpp)
target_link_libraries(RandomWalk_test ${RANDOMWALK_LIBS})

add_executable(RandomWalkBenchmark benchmark.cpp)	
target_link_libraries(RandomWalkBenchmark ${RANDOMWALK_LIBS})

add_executable(TerminateTest TerminateTest.cpp)

if (CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    CXXTEST_ADD_TEST(RandomWalkTest RandomWalkTest.cc
		${RANDOMWALK_SOURCE_DIR}/src/RandomWalk/EventListener.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RandomWalkTest.h)
    target_link_libraries(RandomWalkTest ${RANDOMWALK_LIBS})

	CXXTEST_ADD_TEST(StateVectorTest StateVectorTest.cc
		${CMAKE_CURRENT_SOURCE_DIR}/StateVectorTest.h)
	target_link_libraries(StateVectorTest ${RANDOMWALK_LIBS})

	CXXTEST_ADD_TEST(DiffusionPropensitiesTest PropensitiesDiffusionTest.cc
		${CMAKE_CURRENT_SOURCE_DIR}/PropensitiesDiffusionTest.h)
	target_link_libraries(DiffusionPropensitiesTest ${RANDOMWALK_LIBS})

	CXXTEST_ADD_TEST(DriftPropensitiesTest PropensitiesDriftTest.cc
		${CMAKE_CURRENT_SOURCE_DIR}/PropensitiesDriftTest.h)
	target_link_libraries(DriftPropensitiesTest ${RANDOMWALK_LIBS})

	CXXTEST_ADD_TEST(AdhesionPropensitiesTest PropensitiesAdhesionTest.cc
		${CMAKE_CURRENT_SOURCE_DIR}/PropensitiesAdhesionTest.h)
	target_link_libraries(AdhesionPropensitiesTest ${RANDOMWALK_LIBS})

endif()


