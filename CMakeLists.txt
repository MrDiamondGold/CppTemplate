cmake_minimum_required(VERSION 3.14)

set(CPP_PROJECT_NAME CppTemplate)

project(${CPP_PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(thirdparty/CLI11)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/nativefiledialog-extended)

# IMGUI
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

set(IMGUI_SOURCES
   "thirdparty/imgui/imgui_demo.cpp"
   "thirdparty/imgui/imgui_draw.cpp"
   "thirdparty/imgui/imgui_widgets.cpp"
   "thirdparty/imgui/imgui_tables.cpp"
   "thirdparty/imgui/imgui.cpp"
)

set(IMGUI_IMPL_SOURCES
   "thirdparty/imgui/backends/imgui_impl_glfw.cpp"
   "thirdparty/imgui/backends/imgui_impl_opengl3.cpp"
)

set(IMGUI_INCLUDE_DIRS
   "thirdparty/imgui"
   "thirdparty/imgui/backends"
)

# CPP PROJECT
set(CPP_PROJECT_SOURCES
   "src/glad.c"
   "src/gltf.cpp"
   "src/main.cpp"
   "src/state.cpp"
   "src/program.cpp"
   "src/resources/shader.cpp"
   "src/resources/texture.cpp"
   "src/resources/resources.cpp"
   "src/resources/gltf_scene.cpp"
)

set(CPP_PROJECT_INCLUDE_DIRS
   "include"
   "include/actions"
   "include/resources"
)

file(MAKE_DIRECTORY build)

add_executable(${CPP_PROJECT_NAME} ${CPP_PROJECT_SOURCES} ${IMGUI_SOURCES} ${IMGUI_IMPL_SOURCES})
set_target_properties(${CPP_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(${CPP_PROJECT_NAME} PRIVATE)

target_include_directories(${CPP_PROJECT_NAME} PRIVATE ${IMGUI_INCLUDE_DIRS} ${CPP_PROJECT_INCLUDE_DIRS})
target_link_libraries(${CPP_PROJECT_NAME} PRIVATE CLI11 glfw glm::glm nfd OpenGL::GL)