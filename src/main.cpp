#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <CLI/CLI.hpp>
#include <chrono>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <nfd.hpp>
#include <sstream>
#include <string>

#include "gltf.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "resources.hpp"
#include "texture.hpp"
#include "utility.hpp"

const std::string title = "Template";

GLFWwindow* window;

struct CmdArgs {
    int width, height;
};

void window_resize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void APIENTRY gl_error(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* user_params) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void init(CmdArgs arguments) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(arguments.width, arguments.height, title.c_str(), NULL, NULL);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create glfw window.");
    }

    glfwSetWindowSizeLimits(window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load OpenGL context.");
    }
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_error, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSwapInterval(0);
    glfwSetWindowSizeCallback(window, window_resize);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

int main(int argc, char** argv) {
    CLI::App app{title.c_str()};

    CmdArgs arguments = {1280, 720};

    app.add_option("--width", arguments.width, "");
    app.add_option("--height", arguments.height, "");

    CLI11_PARSE(app, argc, argv);

    NFD::Guard gaurd;

    try {
        init(arguments);

        auto& resources = Resources::get_singleton();
        auto shaders = resources.get_resource<Shader>();

        Program program = Program(shaders.at("default.vert.glsl").data.c_str(), shaders.at("default.frag.glsl").data.c_str());

        glViewport(0, 0, arguments.width, arguments.height);

        auto view_matrix = glm::lookAt(glm::vec3(0.75f, 0.75f, 0.75f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        auto projection_matrix = glm::perspective(glm::radians(45.0f), (float)arguments.width / (float)arguments.height, 0.01f, 100000.0f);

        auto last_time = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::high_resolution_clock::duration::zero();
        while (!glfwWindowShouldClose(window)) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            float fps = (float)(1000000000.0 / delta.count());

            bool metrics_window = true;
            ImGui::ShowMetricsWindow(&metrics_window);

            ImGui::Render();

            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            // for (const auto& primitive : primitives) {
            //     program.bind();
            //     program.set_matrix("view", view_matrix);
            //     program.set_matrix("projection", projection_matrix);

            //     // glBindTexture(GL_TEXTURE_2D, textures.at("leather_mipped.ktx").id);
            //     glBindTexture(GL_TEXTURE_2D, primitive.second.material.albedo.id);
            //     glBindVertexArray(primitive.second.VAO);
            //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.second.EBO);
            //     glDrawElements(primitive.second.mode, primitive.second.size, primitive.second.EBO_type, (GLsizei)0);
            // }

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();

            auto now = std::chrono::high_resolution_clock::now();
            delta = now - last_time;
            last_time = now;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        throw e;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}