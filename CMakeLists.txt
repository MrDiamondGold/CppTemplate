cmake_minimum_required(VERSION 3.14)

set(CPP_PROJECT_NAME CppTemplate)

project(${CPP_PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(Ktx REQUIRED)

add_subdirectory(thirdparty/CLI11)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)
# add_subdirectory(thirdparty/gravity)

set(IMGUI_IMPLEMENTATION_SOURCES thirdparty/imgui/examples/imgui_impl_glfw.cpp thirdparty/imgui/examples/imgui_impl_opengl3.cpp)

file(GLOB IMGUI_SOURCES thirdparty/imgui/*.cpp thirdparty/imgui/*.h)
file(GLOB SOURCE_FILES src *.cpp *.h *.c *.hpp */*.h */*.cpp */*.c */*.hpp)

add_executable(${CPP_PROJECT_NAME} ${SOURCE_FILES} ${IMGUI_SOURCES} ${IMGUI_IMPLEMENTATION_SOURCES})

set_target_properties(${CPP_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(MSVC)
    
else()
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra")
endif()

target_compile_options(${CPP_PROJECT_NAME} PRIVATE -DUNICODE)

target_include_directories(${CPP_PROJECT_NAME} PRIVATE include thirdparty/imgui thirdparty/imgui/examples ${OPENGL_INCLUDE_DIR})

target_link_libraries(${CPP_PROJECT_NAME} PRIVATE CLI11 glfw glm::glm KTX::ktx ${OPENGL_LIBRARY})