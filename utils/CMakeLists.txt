# Utils cmake

set(UTILS_SRC format.cc Log.cpp BasicException.cpp BasicFileLocation.cpp
	dynamic_assert.cpp)

include_directories(${RANDOMWALK_SOURCE_DIR}/utils)
add_library(Utils SHARED ${UTILS_SRC})
