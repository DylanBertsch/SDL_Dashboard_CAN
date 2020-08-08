cmake_minimum_required (VERSION 2.8.3)

project("SDL_Dashboard")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required libraries for linking against nngui (all targets)
set(NNGUI_EXTRA_LIBS "")

# Platform-dependent files for nngui
set(NNGUI_EXTRA_SOURCE "")

include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4100 /wd4101 /wd4018 /wd4201 /wd4189 /wd4457 /wd4456 /wd4245 /wd4838 /wd4389 /wd4702")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- requires C++11 support!")
  endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl/cmake" ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2IMAGE REQUIRED)
find_package(SDL2TTF REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2TTF_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

# Required core libraries on various platforms
if (WIN32) 
  list(APPEND NNGUI_EXTRA_LIBS opengl32)
elseif (APPLE)
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  list(APPEND NNGUI_EXTRA_LIBS ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  list(APPEND NNGUI_EXTRA_SOURCE darwin.mm)
elseif(CMAKE_SYSTEM MATCHES "Linux")
  list(APPEND NNGUI_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl rt)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR})

message(${NNGUI_EXTRA_LIBS})

set(NNGUI_BASIC_SOURCE "" canBus_Comms.h)
list(APPEND NNGUI_BASIC_SOURCE
     nanogui-sdl/sdlgui/button.h
        nanogui-sdl/sdlgui/checkbox.h
        nanogui-sdl/sdlgui/colorpicker.h
        nanogui-sdl/sdlgui/colorwheel.h
        nanogui-sdl/sdlgui/combobox.h
        nanogui-sdl/sdlgui/common.h
        nanogui-sdl/sdlgui/graph.h
        nanogui-sdl/sdlgui/imagepanel.h
        nanogui-sdl/sdlgui/imageview.h
        nanogui-sdl/sdlgui/label.h
        nanogui-sdl/sdlgui/layout.h
        nanogui-sdl/sdlgui/messagedialog.h
        nanogui-sdl/sdlgui/popup.h
        nanogui-sdl/sdlgui/popupbutton.h
        nanogui-sdl/sdlgui/progressbar.h
        nanogui-sdl/sdlgui/screen.h
        nanogui-sdl/sdlgui/slider.h
        nanogui-sdl/sdlgui/stackedwidget.h
        nanogui-sdl/sdlgui/tabheader.h
        nanogui-sdl/sdlgui/tabwidget.h
        nanogui-sdl/sdlgui/textbox.h
        nanogui-sdl/sdlgui/theme.h
        nanogui-sdl/sdlgui/vscrollpanel.h
        nanogui-sdl/sdlgui/switchbox.h
        nanogui-sdl/sdlgui/dropdownbox.h
        nanogui-sdl/sdlgui/widget.h
        nanogui-sdl/sdlgui/window.h
        nanogui-sdl/sdlgui/nanovg.h
        nanogui-sdl/sdlgui/nanovg_rt.h

        nanogui-sdl/sdlgui/button.cpp
        nanogui-sdl/sdlgui/checkbox.cpp
        nanogui-sdl/sdlgui/colorpicker.cpp
        nanogui-sdl/sdlgui/colorwheel.cpp
        nanogui-sdl/sdlgui/combobox.cpp
        nanogui-sdl/sdlgui/common.cpp
        nanogui-sdl/sdlgui/graph.cpp
        nanogui-sdl/sdlgui/imagepanel.cpp
        nanogui-sdl/sdlgui/imageview.cpp
        nanogui-sdl/sdlgui/label.cpp
        nanogui-sdl/sdlgui/layout.cpp
        nanogui-sdl/sdlgui/loadimages.cpp
        nanogui-sdl/sdlgui/messagedialog.cpp
        nanogui-sdl/sdlgui/resources.cpp
        nanogui-sdl/sdlgui/popup.cpp
        nanogui-sdl/sdlgui/popupbutton.cpp
        nanogui-sdl/sdlgui/progressbar.cpp
        nanogui-sdl/sdlgui/screen.cpp
        nanogui-sdl/sdlgui/slider.cpp
        nanogui-sdl/sdlgui/stackedwidget.cpp
        nanogui-sdl/sdlgui/tabheader.cpp
        nanogui-sdl/sdlgui/tabwidget.cpp
        nanogui-sdl/sdlgui/textbox.cpp
        nanogui-sdl/sdlgui/switchbox.cpp
        nanogui-sdl/sdlgui/dropdownbox.cpp
        nanogui-sdl/sdlgui/theme.cpp
        nanogui-sdl/sdlgui/vscrollpanel.cpp
        nanogui-sdl/sdlgui/widget.cpp
        nanogui-sdl/sdlgui/window.cpp
        nanogui-sdl/sdlgui/nanovg.c
)
     
#option(NANOGUI_BUILD_EXAMPLE "Build example application" OFF)
#if (NANOGUI_BUILD_EXAMPLE)
  # Build example application if desired
 # add_executable(example1 ${NNGUI_EXTRA_SOURCE} ${NNGUI_BASIC_SOURCE} example1.cpp )
 # set_target_properties(
  #    example1 PROPERTIES
   #   VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
  #target_link_libraries(example1 ${NNGUI_EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})

  # Copy icons for example application
  if (WIN32) 
    file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/debug)
  else()
    file(COPY nanogui-sdl/resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()
#endif()


set(CMAKE_CXX_STANDARD 14)
include_directories(/home/dylan/Desktop/SDL_Dashboard/kiss_sdl-master)
include_directories(/usr/local/include/SDL2/)
FILE(GLOB MyCSources *.c)
add_executable(SDL_Dashboard main.cpp gui_library.cpp gui_library.h ${NNGUI_EXTRA_SOURCE} ${NNGUI_BASIC_SOURCE})

target_link_libraries(SDL_Dashboard SDL2)
target_link_libraries(SDL_Dashboard SDL2_ttf)
target_link_libraries(SDL_Dashboard SDL2_image)
target_link_libraries(SDL_Dashboard SDL2_gfx)
target_link_libraries(SDL_Dashboard ${NNGUI_EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})
