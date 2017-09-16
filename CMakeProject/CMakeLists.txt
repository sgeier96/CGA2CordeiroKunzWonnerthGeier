#minimum version of cmake
cmake_minimum_required(VERSION 3.2)

# ----------------------------------------------- PROJECT -----------------------------------------------------------------------------------------------------------

#project name
set(PROJ_NAME "CGA2Framework" CACHE STRING "Projektname")
project(${PROJ_NAME})

#variable for collecting all targets
set(TARGETS "")

# ----------------------------------------------- LIBRARIES --------------------------------------------------------------------------------------------------------

# LIB_INCLUDES : All include paths for the linked libraries
set(LIB_INCLUDES "")
# LIB_LINKS : All libraries to be linked
set(LIB_LINKS "")
# LIB_SOURCE_ONLY_FILES : Library source files to be added to the project without linking
set(LIB_SOURCE_ONLY_FILES "")

# optional lib includes

set(INCLUDE_SOIL_LIB ON CACHE BOOL "Include SOIL library")

# ------------------------------------------------- GLFW ----------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.2.1")

#glfw config stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(TARGET glfw)
	list(APPEND LIB_LINKS glfw)
endif()

list(APPEND LIB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.2.1/include" "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.2.1/include/GLFW")

if(TARGET glfw)
	list(APPEND TARGETS glfw)
endif()

if(TARGET uninstall)
	list(APPEND TARGETS uninstall)
endif()

if(TARGET glfw)
	set_target_properties(glfw PROPERTIES FOLDER libs/glfw)
endif(TARGET glfw)

if(TARGET uninstall)
	set_target_properties(uninstall PROPERTIES FOLDER libs/glfw EXCLUDE_FROM_ALL 1)
endif(TARGET uninstall)

# ------------------------------------------------ GLEW ----------------------------------------------------

#problematic glew thing
set(GLEW_SOURCE_ONLY ON CACHE BOOL "Include GLEW as source only")

set(BUILD_UTILS OFF CACHE BOOL "" FORCE)

#glew config stuff
if(NOT GLEW_SOURCE_ONLY)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.0.0/build/cmake")
	if(TARGET glew_s)
		list(APPEND LIB_LINKS glew_s)
	endif()
else(NOT GLEW_SOURCE_ONLY)
	list(APPEND LIB_SOURCE_ONLY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.0.0/include/GL/glew.h" "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.0.0/src/glew.c")
	add_definitions(-DGLEW_STATIC)
endif(NOT GLEW_SOURCE_ONLY)

list(APPEND LIB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.0.0/include" "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.0.0/include/GL")

if(TARGET glew)
	list(APPEND TARGETS glew)
endif()

if(TARGET glew_s)
	list(APPEND TARGETS glew_s)
endif()

if(TARGET glewinfo)
	list(APPEND TARGETS glewinfo)
endif()

if(TARGET visualinfo)
	list(APPEND TARGETS visualinfo)
endif()

if(TARGET glew)
	set_target_properties(glew PROPERTIES FOLDER libs/glew EXCLUDE_FROM_ALL 1)
endif(TARGET glew)

if(TARGET glew_s)
	set_target_properties(glew_s PROPERTIES FOLDER libs/glew)
endif(TARGET glew_s)

if(TARGET glewinfo)
	set_target_properties(glewinfo PROPERTIES FOLDER libs/glew)
endif(TARGET glewinfo)

if(TARGET visualinfo)
	set_target_properties(visualinfo PROPERTIES FOLDER libs/glew)
endif(TARGET visualinfo)

# ----------------------------------------------- OPENGL AND GLU (IF NOT INCLUDED BY GLEW) ------------------------

if(NOT OPENGL_FOUND)
	find_package(OpenGL REQUIRED)	
	if(NOT OPENGL_FOUND)
		message(FATAL_ERROR "OpenGL not found")
	else()
		list(APPEND LIB_LINKS ${OPENGL_LIBRARIES})
	endif()
else()
	message(STATUS "OpenGL is already included by GLEW or GLFW")
endif()

# ----------------------------------------------- GLM -------------------------------------------------------

list(APPEND LIB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm-0.9.8.4/glm" "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm-0.9.8.4/glm/glm")

# ---------------------------------------------- SOIL ---------------------------------------------------------

if(INCLUDE_SOIL_LIB)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/soil")	
	if(SOIL_LIBRARY)
		if(TARGET ${SOIL_LIBRARY})
			list(APPEND LIB_INCLUDES ${SOIL_INCLUDES})
			list(APPEND LIB_LINKS ${SOIL_LIBRARY})
			list(APPEND TARGETS ${SOIL_LIBRARY})
			set_target_properties(${SOIL_LIBRARY} PROPERTIES FOLDER libs/SOIL)
		endif()
	else()
		message(FATAL_ERROR "SOIL could not be included.")
	endif()
endif()

# ---------------------------------------------- PROJECT SOURCES -----------------------------------------------

#sources and headers

#framework
set(FW_SOURCES "${CMAKE_SOURCE_DIR}/framework/GameWindow.cpp" "${CMAKE_SOURCE_DIR}/framework/glerror.cpp" "${CMAKE_SOURCE_DIR}/framework/Input.cpp" "${CMAKE_SOURCE_DIR}/framework/OBJLoader.cpp")
set(FW_HEADERS "${CMAKE_SOURCE_DIR}/framework/GameWindow.h" "${CMAKE_SOURCE_DIR}/framework/glerror.h" "${CMAKE_SOURCE_DIR}/framework/Input.h" "${CMAKE_SOURCE_DIR}/framework/libheaders.h" "${CMAKE_SOURCE_DIR}/framework/OBJLoader.h" "${CMAKE_SOURCE_DIR}/framework/fw_config.h")
set(FW_INCLUDES "${CMAKE_SOURCE_DIR}/framework")

#sources and headers from src directory
file(GLOB_RECURSE SRC_HEADERS "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE SRC_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

#find include directories
#src directory should always be included

set(SRC_INCLUDES "${CMAKE_SOURCE_DIR}/src")

foreach(pth ${SRC_HEADERS})
	get_filename_component(hDIR ${pth} DIRECTORY)
	list(FIND SRC_INCLUDES ${hDIR} fINDEX)
	if(${fINDEX} EQUAL -1)
		list(APPEND SRC_INCLUDES "${hDIR}")
	endif()
endforeach()

set(PROJ_INCLUDES ${FW_INCLUDES} ${SRC_INCLUDES})
set(PROJ_SOURCES  ${FW_SOURCES} ${SRC_SOURCES})
set(PROJ_HEADERS ${FW_HEADERS} ${SRC_HEADERS})

# ------------------------------------------------ EXECUTABLE TARGET -----------------------------------------------------------

add_executable(${PROJ_NAME} ${PROJ_HEADERS} ${PROJ_SOURCES} ${LIB_SOURCE_ONLY_FILES})

list(APPEND TARGETS ${PROJ_NAME})

set(ALL_INCLUDES ${LIB_INCLUDES} ${PROJ_INCLUDES})

target_link_libraries(${PROJ_NAME} ${LIB_LINKS})

include_directories(${ALL_INCLUDES})

# ------------------------------------------------------- DEUGLIFY PROJECT -------------------------------------------------

# ------------------------------------------------------ GROUP SOURCES ---------------------------------------------

# ----------------------------------------------------- FRAMEWORK GROUP --------------------------------------------

source_group(framework FILES ${FW_HEADERS} ${FW_SOURCES})

# ----------------------------------------------------- SRC GROUP -----------------------------------------------------------

source_group(src FILES ${SRC_HEADERS} ${SRC_SOURCES})

# ----------------------------------------------------- SOURCE ONLY LIB GROUP -----------------------------------------------

if(LIB_SOURCE_ONLY_FILES)
	list(LENGTH LIB_SOURCE_ONLY_FILES LSO_COUNT)

	if(${LSO_COUNT} GREATER 0)
		source_group(lib_source_only FILES ${LIB_SOURCE_ONLY_FILES})
	endif()
endif()


# ------------------------------------------ OUTPUT DIRECTORIES -------------------------------------------------

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")

foreach(tg ${TARGETS})
	set_target_properties(${tg} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
	
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		set(CFG_NAME ${OUTPUTCONFIG})
   		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
	
		set_target_properties(${tg} PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${OUTPUT_DIR}/${CFG_NAME}"
			LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${OUTPUT_DIR}/${CFG_NAME}"
			RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${OUTPUT_DIR}/${CFG_NAME}")
	endforeach()
endforeach()

# ----------------------------------------- IDE/COMPILER/PLATFORM SPECIFIC ----------------------------------------------------------------

if(MSVC)
	#Debugger working directory
	set_target_properties(${PROJ_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${OUTPUT_DIR}/Debug")

	#startup project DOES NOT WORK CURRENTLY
	#set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})
endif()

#----------------------------------------- LANGUAGE LEVEL REQUIREMENTS --------------------------------------------------------------------

# force c++11 for main project
set_target_properties(${PROJ_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON CXX_STANDARD 11)