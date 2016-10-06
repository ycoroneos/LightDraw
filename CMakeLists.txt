cmake_minimum_required(VERSION 2.8)

project(voxl)

find_package(OpenGL REQUIRED)

if (APPLE)
  set(CMAKE_MACOSX_RPATH 1)
endif()

if (UNIX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++11")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")
  # recommended but not set by default
  # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
elseif(MSVC)
  # recommended but not set by default
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
endif()

set (VOXL_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(glfw)
list(APPEND VOXL_LIBS glfw)
list(APPEND VOXL_INCLUDES glfw/include)

# GLEW - not needed on OS X
if (NOT APPLE)
  find_package(GLEW REQUIRED)
    if (GLEW_FOUND)
      list(APPEND VOXL_INCLUDES ${GLEW_INCLUDE_DIRS})
      list(APPEND VOXL_LIBS ${GLEW_LIBRARIES})
    endif()
endif()

#GLM
list(APPEND VOXL_INCLUDES glm/glm)

## vecmath include directory
#include_directories(vecmath/include)
#add_subdirectory(vecmath)
#list (APPEND A2_LIBS vecmath)
#list (APPEND A2_INCLUDES vecmath/include)
list (APPEND VOXL_SRC
  src/main.cpp
)
list (APPEND VOXL_HEADER
)

add_executable(voxl ${VOXL_SRC} ${VOXL_HEADER})
target_include_directories(voxl PUBLIC ${VOXL_INCLUDES})
target_link_libraries(voxl ${VOXL_LIBS})
