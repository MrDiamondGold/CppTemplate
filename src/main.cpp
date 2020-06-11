//#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <ixwebsocket/IXNetSystem.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include <CLI/CLI.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <strstream>

struct CmdArgs {
    int width, height;
};

int main(int argc, char** argv) {
    CLI::App app{"Cpp Template"};

    CmdArgs arguments = {1920, 1080};

    app.add_option("--width", arguments.width, "");
    app.add_option("--height", arguments.height, "");

    CLI11_PARSE(app, argc, argv);

    std::string title = "Template";

#ifdef _WIN32
    ix::initNetSystem();
#endif

    if (!glfwInit()) {
        // ERROR
    }

    GLFWwindow *window = glfwCreateWindow(arguments.width, arguments.height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        // ERROR
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Example");
        ImGui::Text("Test");
        ImGui::End();

        ImGui::Render();

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        glViewport(0, 0, w, h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

#ifdef _WIN32
    ix::uninitNetSystem();
#endif

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}