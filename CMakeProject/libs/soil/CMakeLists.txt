#minimum version of cmake
cmake_minimum_required(VERSION 3.2)

# ----------------------------------------------- PROJECT -----------------------------------------------------------------------------------------------------------

#project name
project("SOIL")

# ----------------------------------------------- Library source files ----------------------------------------------------------------------------------------------
set(SOIL_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/src/image_DXT.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/image_helper.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_aug.c")
                
set(SOIL_HEADERS    "${CMAKE_CURRENT_SOURCE_DIR}/src/image_DXT.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src/image_helper.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_aug.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src/stbi_DDS_aug.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src/stbi_DDS_aug_c.h")

# ----------------------------------------------- library includes --------------------------------------------------------------------------------------------------

set(SOIL_INCLUDES_PRIV "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOIL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/src" PARENT_SCOPE)

# ----------------------------------------------- dependencies ------------------------------------------------------------------------------------------------------

#if(NOT OPENGL_FOUND)
#	find_package(OpenGL REQUIRED)	
#	if(NOT OPENGL_FOUND)
#		message(FATAL_ERROR "OpenGL not found")
#	endif()
#else()
#	message(STATUS "OpenGL is already included")
#endif()

# ----------------------------------------------- targets -----------------------------------------------------------------------------------------------------------

add_library(SOIL STATIC ${SOIL_HEADERS} ${SOIL_SRC})

if(TARGET SOIL)
    set(SOIL_LIBRARY SOIL PARENT_SCOPE)
endif()