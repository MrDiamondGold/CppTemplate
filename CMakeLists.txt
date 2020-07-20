cmake_minimum_required(VERSION 3.14)

set(CPP_PROJECT_NAME CppTemplate)

project(${CPP_PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

add_subdirectory(thirdparty/CLI11)
add_subdirectory(thirdparty/glfw)

file(GLOB IMGUI_SOURCES thirdparty/imgui thirdparty/imgui/examples *.c *.cpp)
file(GLOB SOURCES src *.c *.cpp)

add_executable(${CPP_PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

if(MSVC)
    
else()
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra")
endif()

target_compile_options(${CPP_PROJECT_NAME} PRIVATE -DUNICODE)

target_include_directories(${CPP_PROJECT_NAME} PRIVATE include)
target_include_directories(${CPP_PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR})

target_link_libraries(${CPP_PROJECT_NAME} PRIVATE CLI11)
target_link_libraries(${CPP_PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${CPP_PROJECT_NAME} PRIVATE ${OPENGL_LIBRARY})