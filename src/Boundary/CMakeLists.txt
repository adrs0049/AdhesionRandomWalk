# Cmake file for Boundary

set(BOUNDARY_SOURCES PeriodicBoundary.cpp)
include_directories(${RANDOMWALK_SOURCE_DIR}/core)

add_library(Boundary SHARED ${BOUNDARY_SOURCES})
