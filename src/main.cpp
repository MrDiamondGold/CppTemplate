//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <CLI/CLI.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include "mesh.hpp"

const std::string title = "Template";

GLFWwindow* window;

struct CmdArgs {
    int width, height;
};

void init(CmdArgs arguments) {
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(arguments.width, arguments.height, title.c_str(), NULL, NULL);
    if(window == nullptr) {
        throw std::runtime_error("Failed to create glfw window.");
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGL()) {
        throw std::runtime_error("Failed to load OpenGL context.");
    }
}

int main(int argc, char** argv) {
    CLI::App app{title.c_str()};

    CmdArgs arguments = {640, 480};

    app.add_option("--width", arguments.width, "");
    app.add_option("--height", arguments.height, "");

    CLI11_PARSE(app, argc, argv);

    try {
        init(arguments);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        while(!glfwWindowShouldClose(window))
        {
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    glfwTerminate();

    return 0;
}