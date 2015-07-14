# test cmake

# find cxxtest
find_package(CxxTest)

include_directories(
    ${RANDOMWALK_SOURCE_DIR}
    ${RANDOMWALK_SOURCE_DIR}/utils
	${RANDOMWALK_SOURCE_DIR}/utils/vector_simd)

if (CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()
	CXXTEST_ADD_TEST(Vector4fTest Vector4fTest.cc 
		${RANDOMWALK_SOURCE_DIR}/utils/aligned_allocator_detail.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vector4fTest.h)

        CXXTEST_ADD_TEST(Vector2dTest Vector2dTest.cc
		${RANDOMWALK_SOURCE_DIR}/utils/aligned_allocator_detail.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vector2dTest.h)

        CXXTEST_ADD_TEST(Vector8fTest Vector8fTest.cc
		${RANDOMWALK_SOURCE_DIR}/utils/aligned_allocator_detail.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vector8fTest.h)

        CXXTEST_ADD_TEST(Vector4dTest Vector4dTest.cc
		${RANDOMWALK_SOURCE_DIR}/utils/aligned_allocator_detail.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vector4dTest.h)

	#CXXTEST_ADD_TEST(Vector4iTest Vector4iTest.cc 
	#	${RANDOMWALK_SOURCE_DIR}/utils/aligned_allocator_detail.cpp
	#    ${CMAKE_CURRENT_SOURCE_DIR}/Vector4iTest.h)

endif()


