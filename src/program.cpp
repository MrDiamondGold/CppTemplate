#include "program.h"

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

        std::cout << log.data() << '\n';
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

        std::cout << log.data() << '\n';
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

void Program::set_int(const std::string name, int value) const {
    unsigned int int_location = glGetUniformLocation(this->id, name.c_str());
    glUniform1i(int_location, value);
}

void Program::set_float(const std::string name, float value) const {
    unsigned int float_location = glGetUniformLocation(this->id, name.c_str());
    glUniform1f(float_location, value);
}

void Program::set_vec3(const std::string name, glm::vec3 value) const {
    unsigned int vec3_location = glGetUniformLocation(this->id, name.c_str());
    glUniform3fv(vec3_location, 1, glm::value_ptr(value));
}

void Program::set_vec4(const std::string name, glm::vec4 value) const {
    unsigned int vec4_location = glGetUniformLocation(this->id, name.c_str());
    glUniform4fv(vec4_location, 1, glm::value_ptr(value));
}

void Program::set_matrix(const std::string name, glm::mat4 matrix) const {
    unsigned int matrix_location = glGetUniformLocation(this->id, name.c_str());
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::bind() const {
    glUseProgram(this->id);
}