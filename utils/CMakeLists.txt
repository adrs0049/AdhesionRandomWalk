# Utils cmake

add_subdirectory(vector_simd)
add_subdirectory(spdlog)

set(UTILS_SRC format.cc Log.cpp BasicException.cpp BasicFileLocation.cpp dynamic_assert.cpp aligned_allocator_detail.cpp)

include_directories(${RANDOMWALK_SOURCE_DIR}/utils)
add_library(Utils SHARED ${UTILS_SRC})
