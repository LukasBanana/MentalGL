
# === CMake lists for MentalGL - (26/03/2018) ===

cmake_minimum_required(VERSION 2.8)
project(MentalGL)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)


# === Test Projects ===

find_package(OpenGL)
find_package(GLUT)

add_library(MentalGL STATIC "${PROJECT_SOURCE_DIR}/mental_gl.h")
set_target_properties(MentalGL PROPERTIES LINKER_LANGUAGE C DEBUG_POSTFIX "D")

if(OpenGL_FOUND AND GLUT_FOUND)
	include_directories(
		"${GLUT_INCLUDE_DIR}"
		"${PROJECT_SOURCE_DIR}/thirdparty/glad/include"
	)
	
	# Test 1
	add_executable(
		test1
		"${PROJECT_SOURCE_DIR}/test1.c"
		"${PROJECT_SOURCE_DIR}/thirdparty/glad/src/glad.c"
		"${PROJECT_SOURCE_DIR}/thirdparty/glad/include/glad/glad.h"
	)
	set_target_properties(test1 PROPERTIES LINKER_LANGUAGE C DEBUG_POSTFIX "D")
	target_link_libraries(test1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
else()
	message("missing OpenGL and/or GLUT to generate tests")
endif()


