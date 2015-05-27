project(randomwalk)

# cmake min version
cmake_minimum_required(VERSION 2.8)

# CFLAGS
# if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "DEBUG")
#set(CMAKE_BUILD_TYPE "RELEASE")
#set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
#set(CMAKE_BUILD_TYPE "MINSIZEREL")
# endif()

set(CMAKE_CXX_FLAGS_C11 " -std=c++11 -march=native")
set(CMAKE_CXX_COMMON_APPEND "${CMAKE_CXX_FLAGS_C11} -D__STRICT_ANSI__")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_COMMON_APPEND} -O2 -DNDEBUG_BUILD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_COMMON_APPEND} -DDEBUG_BUILD -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  ${CMAKE_CXX_COMMON_APPEND} -DNDEBUG_BUILD" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_COMMON_APPEND} -O3 -DNDEBUG_BUILD")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if ( CMAKE_COMPILER_IS_GNUCC )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
	set(CMAKE_EXE_LINKER_FLAGS "-pg")
endif(CMAKE_COMPILER_IS_GNUCC)

option(USE_C_ABI_INTERFACE "Compile the C interface that is required for python ctypes" ON)
if (USE_C_ABI_INTERFACE)
	add_definitions("-DUSE_C_INTERFACE")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
 # add_definitions("-std=c++11 -Wall -Wextra -Wshadow -Weffc++ -pedantic -Wstrict-aliasing -Wpointer-arith -Wcast-qual")
add_definitions("-std=c++11")
 else()
  message(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
endif()

# output directories
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

#find_package(PythonLibs 2.7 REQUIRED)
#include_directories("${PYTHON_INCLUDE_DIRS}")

find_package (Boost)
if (Boost_FOUND)
	include_directories("${Boost_INCLUDE_DIRS}")
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	#find_package(Boost COMPONENTS python unit_test_framework REQUIRED)
	find_package(Boost COMPONENTS  unit_test_framework REQUIRED)
elseif(NOT Boost_FOUND)
	message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

find_package(SWIG REQUIRED)
include(UseSWIG)
include_directories(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

set(CMAKE_SWIG_FLAGS "")

# add main include directory
include_directories("include/")

#set(RANDOM_SRC_FILES ParkMiller.cpp)
#add_library(Random SHARED ${RANDOM_SRC_FILES})
#target_link_libraries(Random ${PYTHON_LIBRARY} ${Boost_LIBRARIES})

set(BOUNDARY_SRC_FILES PeriodicBoundary.cpp)
add_library(Boundary SHARED ${BOUNDARY_SRC_FILES})

add_executable(RandomWalk_test main.cpp BasicException.cpp
BasicFileLocation.cpp)
target_link_libraries(RandomWalk_test Boundary  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(RandomWalkBenchmark benchmark.cpp BasicException.cpp
BasicFileLocation.cpp)
target_link_libraries(RandomWalkBenchmark Boundary)

add_executable(CellDensity_test CellDensityTest.cpp BasicException.cpp
BasicFileLocation.cpp)
target_link_libraries(CellDensity_test Boundary  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

set(RANDOMWALK_SRC_FILES Simulator.cpp BasicException.cpp BasicFileLocation.cpp)
add_library(RandomWalk SHARED ${RANDOMWALK_SRC_FILES})
target_link_libraries(RandomWalk Boundary)

set_source_files_properties(simulator.i PROPERTIES CPLUSPLUS ON)
#set_source_files_properties(simulator.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(simulator python simulator.i)
swig_link_libraries(simulator RandomWalk Boundary ${PYTHON_LIBRARIES})

#install(TARGETS randomwalk RUNTIME DESTINATION bin)
