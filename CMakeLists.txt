cmake_minimum_required(VERSION 3.14)

set(CPP_PROJECT_NAME CppTemplate)

project(${CPP_PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    #set( CMAKE_CXX_FLAGS "/Wall" )
else()
    set( CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra" )
endif()


#include(ExternalProject)
include(FetchContent)

FetchContent_Declare(
  ixwebsocket
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
  GIT_TAG v9.1.9
)

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v1.9.0
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.2
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.76
)

FetchContent_MakeAvailable(ixwebsocket CLI11 glfw imgui)

add_executable(${CPP_PROJECT_NAME} src/main.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl2.cpp)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_include_directories(${CPP_PROJECT_NAME} PUBLIC include ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/examples)
target_link_libraries(${CPP_PROJECT_NAME} PUBLIC ixwebsocket CLI11 glfw ${OPENGL_LIBRARY} )