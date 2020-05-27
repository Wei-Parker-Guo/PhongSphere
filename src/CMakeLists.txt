# Original file Copyright CMU462 Fall 2015: 
# Kayvon Fatahalian, Keenan Crane,
# Sky Gao, Bryce Summers, Michael Choquette.
cmake_minimum_required(VERSION 2.8)

# Application source

# enable this to build starter code
# set(APPLICATION_SOURCE
    
#     example_01.cpp
# )

set(ASSESSMENT1_SOURCE assessment_1.cpp)

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
#  ${FREETYPE_INCLUDE_DIRS} disabled freetype since it causes malfunc on my Win10 system
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
#  ${CGL_LIBRARY_DIRS}
#  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------

# enable this to build starter code
# add_executable(example1 ${APPLICATION_SOURCE})
# target_link_libraries( example1
#     glew ${GLEW_LIBRARIES}
#     glfw ${GLFW_LIBRARIES}
#     ${OPENGL_LIBRARIES}
# #    ${FREETYPE_LIBRARIES}
#     ${CMAKE_THREADS_INIT}
# )


add_executable(as1 ${ASSESSMENT1_SOURCE})

target_link_libraries( as1
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
#    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET as1 APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS as1 DESTINATION ${Assignment1_SOURCE_DIR})
