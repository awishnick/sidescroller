# This is based off http://immersedcode.org/2011/4/6/sdl13-intro/
cmake_minimum_required(VERSION 2.6)
project(sidescroller)

set(SOURCES
  src/main.cpp
)
set(HEADERS
)

# We support clang
option(USE_CLANG "build with clang/libcxx" ON)
if(USE_CLANG)
  set(CMAKE_CXX_COMPILER /usr/bin/clang++)
  set(CMAKE_CXX_FLAGS "-stdlib=libc++")
  add_definitions(-Wall -Wextra -Werror -std=c++0x)
endif(USE_CLANG)

# The following code finds SDL in your checkout on OS X, Linux
# as well as Windows.  On Windows I am lazy and only look for the
# release version of SDL, feel free to make this also look for debug
# modes depending on the cmake build target.
set(SDL_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl-1.2.15
    CACHE STRING "Path to SDL 1.2.15" FORCE)
find_library(SDL_LIBRARY
    NAMES SDL-1.2.15 SDL
    PATHS ${SDL_FOLDER}
    PATH_SUFFIXES local/lib VisualC/SDL/Release
    NO_DEFAULT_PATH
)
find_library(SDLMAIN_LIBRARY
    NAMES SDLmain
    PATHS ${SDL_FOLDER}
    PATH_SUFFIXES local/lib VisualC/SDLmain/Release
    NO_DEFAULT_PATH
)

# we also need to find the system's OpenGL version
find_package(OpenGL REQUIRED)

# on OS X we also have to add '-framework Cocoa' as library.  This is
# actually a bit of an hack but it's easy enough and reliable.
set(EXTRA_LIBS "")
if (APPLE)
    set(EXTRA_LIBS ${EXTRA_LIBS} "-framework Cocoa")
endif()

# Add SDL as an include folder
include_directories(
    ${SDL_FOLDER}/include
    src
)
 
add_executable(
  sidescroller
  WIN32
  MACOSX_BUNDLE
  ${SOURCES}
  ${HEADERS}
)

# Lastly we have to link the OpenGL libraries, SDL and the cocoa
# framework to our application.  The latter is only happening on
# OS X obviously.
target_link_libraries(
    sidescroller
    ${OPENGL_LIBRARIES}
    ${SDL_LIBRARY}
    ${SDLMAIN_LIBRARY}
    ${EXTRA_LIBS}
)

