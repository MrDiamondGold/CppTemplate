//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <CLI/CLI.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "shader.hpp"
#include "mesh.hpp"

const std::string title = "Template";

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}

)";

GLFWwindow* window;

struct CmdArgs {
    int width, height;
};

void init(CmdArgs arguments) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(arguments.width, arguments.height, title.c_str(), NULL, NULL);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create glfw window.");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

        Shader shader = Shader(vertexShaderSource, fragmentShaderSource);
        Mesh mesh = Mesh();

        mesh.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
        mesh.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
        mesh.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
        mesh.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));

        mesh.indices.push_back(0);
        mesh.indices.push_back(1);
        mesh.indices.push_back(3);
        mesh.indices.push_back(1);
        mesh.indices.push_back(2);
        mesh.indices.push_back(3);

        mesh.updateMesh();

        glViewport(0, 0, arguments.width, arguments.height);

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shader.bind();
            mesh.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    glfwTerminate();

    return 0;
}