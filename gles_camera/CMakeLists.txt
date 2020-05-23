cmake_minimum_required(VERSION 3.0)
project(gles_camera)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  gles_render_thread
)

catkin_package()

# Try to find libv4l (it's not strictly needed, but is useful for debugging puproses)
find_library(_V4L_LIB v4l2)
find_path(_V4L_INCLUDE libv4l2.h)

if (_V4L_LIB AND _V4L_INCLUDE)
  add_library(V4l INTERFACE)
  target_link_libraries(V4l INTERFACE ${_V4L_LIB})
  target_include_directories(V4l INTERFACE ${_V4L_INCLUDE})
endif()

add_library(v4l2device STATIC
  src/v4l2device.cpp
)

if (TARGET V4l)
  message(STATUS "Building with libv4l (debug purposes only)")
  target_link_libraries(v4l2device PUBLIC V4l)
  target_compile_definitions(v4l2device PRIVATE WITH_LIBV4L)
else()
  message(STATUS "Building without libv4l")
endif()

add_executable(v4l2info src/test/v4l2info.cpp)

target_link_libraries(v4l2info PUBLIC v4l2device ${catkin_LIBRARIES})
target_include_directories(v4l2info PUBLIC ${catkin_INCLUDE_DIRS})

find_library(_SDL2 SDL2)
find_library(_SDL2_MAIN SDL2main)
find_path(_SDL2_INCLUDE SDL2/SDL.h)
if (_SDL2 AND _SDL2_MAIN AND _SDL2_INCLUDE)
  message(STATUS "Found SDL2: ${_SDL2}; ${_SDL2_MAIN}; ${_SDL2_INCLUDE}")
  add_library(Sdl2 INTERFACE)
  target_link_libraries(Sdl2 INTERFACE ${_SDL2} ${_SDL2_MAIN})
  target_include_directories(Sdl2 INTERFACE ${_SDL2_INCLUDE})
endif()

if (TARGET Sdl2)
  message(STATUS "Building SDL2-based preview tool")
  add_executable(sdl_v4l2
    src/test/sdl_v4l2.cpp
  )

  target_link_libraries(sdl_v4l2 PUBLIC Sdl2 v4l2device GLESv2)
else()
  message(STATUS "SDL2 not found, not building preview tool")
endif()
