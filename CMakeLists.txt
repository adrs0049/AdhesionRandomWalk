project(RANDOMWALK)

# cmake min version
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# CFLAGS
if(NOT CMAKE_BUILD_TYPE)
	#set(CMAKE_BUILD_TYPE "DEBUG")
    #set(CMAKE_BUILD_TYPE "RELEASE")
	set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
    #set(CMAKE_BUILD_TYPE "MINSIZEREL")
endif()

set(CMAKE_CXX_FLAGS_C11 " -std=c++11 -march=native -mavx2 -mavx")
set(CMAKE_CXX_COMMON_APPEND "${CMAKE_CXX_FLAGS_C11} -D__STRICT_ANSI__ -Wall -pedantic")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_COMMON_APPEND}")
set(CMAKE_CXX_FLAGS_VALGRIND "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG_BUILD -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG_BUILD -O3" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DDEBUG_BUILD")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-Wall -pedantic")
    # add_definitions("-Wall -Wextra -Wshadow -Weffc++ -pedantic -Wstrict-aliasing -Wpointer-arith -Wcast-qual")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlibc++ -std=c++11")
    message(WARNING "Some features such as profiling are not supported using Clang")
else()
    message(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

add_subdirectory(src)
add_subdirectory(utils)
