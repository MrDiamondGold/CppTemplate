#include "program.hpp"

unsigned int Program::create_program(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);

    glLinkProgram(id);

    GLint isCompiled = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> log = std::vector<GLchar>(maxLength);
        glGetProgramInfoLog(id, maxLength, &maxLength, &log[0]);

        glDeleteProgram(id);

        throw std::runtime_error(log.data());
    }

    return id;
}

unsigned int Program::create_shader(const char* code, GLenum type) {
    unsigned int id = glCreateShader(type);

    glShaderSource(id, 1, &code, nullptr);
    glCompileShader(id);

    GLint isCompiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> log = std::vector<GLchar>(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, &log[0]);

        glDeleteShader(id);

        throw std::runtime_error(log.data());
    }

    return id;
}

Program::Program(const char* vertexCode, const char* fragmentCode) {
    unsigned int vertexShader = this->create_shader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragmentShader = this->create_shader(fragmentCode, GL_FRAGMENT_SHADER);

    this->id = this->create_program(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Program::~Program() {
    glDeleteProgram(this->id);
}

void Program::bind() const {
    glUseProgram(this->id);
}