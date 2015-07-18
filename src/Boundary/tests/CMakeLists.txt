
# find cxxtest
find_package(CxxTest)

include_directories(
    ${RANDOMWALK_SOURCE_DIR}/src/Boundary
	${RANDOMWALK_SOURCE_DIR}/utils)

if (CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()
    CXXTEST_ADD_TEST(PeriodicBoundaryTest PeriodicBoundaryTest.cc
		${CMAKE_CURRENT_SOURCE_DIR}/../PeriodicBoundary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PeriodicBoundaryTest.h)
    target_link_libraries(PeriodicBoundaryTest ${RANDOMWALK_LIBS})
endif()


