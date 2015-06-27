# Utils cmake

set(UTILS_SRC BasicException.cpp BasicFileLocation.cpp)

include_directories(${RANDOMWALK_SOURCE_DIR}/utils)
add_library(Utils SHARED ${UTILS_SRC})
