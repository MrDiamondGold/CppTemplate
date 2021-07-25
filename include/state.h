#pragma once

#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <CLI/CLI.hpp>
#include <iostream>
#include <nfd.hpp>
#include <stdexcept>
#include <thread>

#include "gltf_scene.h"

constexpr char* title = "glTF Converter";

class State {
   private:
    void init_glfw();
    void init_imgui();

    void deinit_glfw();
    void deinit_imgui();

   public:
    GLFWwindow* window;
    GLTFScene* scene;
    ImGuiIO* io;

    bool ktx;

    std::vector<std::thread> toktx_threads;
    size_t total_textures;
    std::atomic_uint32_t completed_textures;

    State(int argc, char** argv);
    ~State();

    void pre_render();
    void render();

    void update();

    void open_gltf_file_dialog();
    void save_gltf_file_dialog();
    void convert_current_file();
    void convert_texture(const std::string command);
};