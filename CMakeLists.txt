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

set (voxl_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(3rd_party/glfw)
list(APPEND voxl_LIBS glfw)
list(APPEND voxl_INCLUDES 3rd_party/glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND voxl_INCLUDES 3rd_party/glew/include)
  list(APPEND voxl_SRC 3rd_party/glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES 3rd_party/glew/src/glew.c)
endif()

#GLM
list(APPEND voxl_INCLUDES 3rd_party/glm)
list(APPEND voxl_INCLUDES .)

list (APPEND voxl_SRC
  src/main.cpp
  src/voxel.cpp
  src/chunk.cpp
  src/glt.cpp
  src/camera.cpp
  src/scene.cpp
  src/input.cpp
)
list (APPEND voxl_HEADER
  inc/voxel.h
  inc/gl.h
  inc/chunk.h
  inc/glt.h
  inc/camera.h
  inc/scene.h
  inc/input.h
)

add_executable(voxl ${voxl_SRC} ${voxl_HEADER})
target_include_directories(voxl PUBLIC ${voxl_INCLUDES})
target_link_libraries(voxl ${voxl_LIBS})
