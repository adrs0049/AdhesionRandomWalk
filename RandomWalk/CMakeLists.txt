project(randomwalk)

# cmake min version
cmake_minimum_required(VERSION 3.0)

# CFLAGS
set(CMAKE_BUILD_TYPE Debug)
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

find_package (Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include
	     "${INCLUDE_DIRECTORIES}/../../array/include"
                     ${Boost_INCLUDE_DIRS}
)

set(SRC_FILES2 RandomBase.cpp ParkMiller.cpp)
add_library(Random ${SRC_FILES2})

add_executable(randomwalk main.cpp)

add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(ParkMiller_test ParkMiller_test.cpp)
target_link_libraries(ParkMiller_test Random ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})  

install(TARGETS randomwalk RUNTIME DESTINATION bin)
