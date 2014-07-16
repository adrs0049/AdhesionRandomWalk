project(randomwalk)

# cmake min version
cmake_minimum_required(VERSION 2.8)

# CFLAGS
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "DEBUG")
  #set(CMAKE_BUILD_TYPE "RELEASE")
  #set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #set(CMAKE_BUILD_TYPE "MINSIZEREL")
endif()

set(CMAKE_CXX_FLAGS_C11 " -std=gnu++11 -march=core-avx2 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_C11} -O2 -DNDEBUG_BUILD")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_C11} -DDEBUG_BUILD -O2 -Wall")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  ${CMAKE_CXX_FLAGS_C11} -DNDEBUG_BUILD" )

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_FLAGS_C11} -O3 -DNDEBUG_BUILD")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")

if ( CMAKE_COMPILER_IS_GNUCC )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
	set(CMAKE_EXE_LINKER_FLAGS "-pg")
endif(CMAKE_COMPILER_IS_GNUCC)

option(USE_C_ABI_INTERFACE "Compile the C interface that is required for python ctypes" ON)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-Wall")
else()
  message(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
endif()

find_package(PythonLibs 2.7 REQUIRED)
include_directories("${PYTHON_INCLUDE_DIRS}")

find_package (Boost)
if (Boost_FOUND)
	include_directories("${Boost_INCLUDE_DIRS}")
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS python unit_test_framework REQUIRED)
elseif(NOT Boost_FOUND)
	message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

# add main include directory
include_directories("include/")

set(RANDOM_SRC_FILES RandomBase.cpp ParkMiller.cpp)
add_library(Random SHARED ${RANDOM_SRC_FILES})
target_link_libraries(Random ${PYTHON_LIBRARY} ${Boost_LIBRARIES})

set(STATISTICS_SRC_FILES StatisticsBase.h RandomWalkStatistics.h RandomWalkStatistics.cpp)
add_library(Statistics SHARED ${STATISTICS_SRC_FILES})

set(RANDOMWALK_SRC_FILES RandomWalkBase.h RandomWalkBase.cpp)
add_library(RandomWalk SHARED ${RANDOMWALK_SRC_FILES})

add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(ParkMiller_test ParkMiller_test.cpp)
target_link_libraries(ParkMiller_test Random ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})  

add_executable(Statistics_test Statistics_test.cpp)
target_link_libraries(Statistics_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(RandomWalk_test RandomWalk_test.cpp)
target_link_libraries(RandomWalk_test RandomWalk Statistics Random ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

#install(TARGETS randomwalk RUNTIME DESTINATION bin)
