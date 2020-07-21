cmake_minimum_required(VERSION 3.14)

set(CPP_PROJECT_NAME CppTemplate)

project(${CPP_PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

add_subdirectory(thirdparty/CLI11)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

file(GLOB IMGUI_SOURCES thirdparty/imgui thirdparty/imgui/examples *.c *.cpp)
file(GLOB SOURCE_FILES src *.cpp *.h *.c *.hpp */*.h */*.cpp */*.c */*.hpp)

add_executable(${CPP_PROJECT_NAME} ${SOURCE_FILES} ${IMGUI_SOURCES})

set_target_properties(${CPP_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(MSVC)
    set_target_properties(${CPP_PROJECT_NAME} PROPERTIES
    LINK_FLAGS /MACHINE:X64
    )
else()
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra")
endif()

target_compile_options(${CPP_PROJECT_NAME} PRIVATE -DUNICODE)

target_include_directories(${CPP_PROJECT_NAME} PRIVATE include)
target_include_directories(${CPP_PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR})

target_link_libraries(${CPP_PROJECT_NAME} PRIVATE CLI11 glfw glm::glm)
target_link_libraries(${CPP_PROJECT_NAME} PRIVATE ${OPENGL_LIBRARY})