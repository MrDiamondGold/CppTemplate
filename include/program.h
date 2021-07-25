#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "glad/glad.h"

class Program {
   protected:
    unsigned int id;

   private:
    unsigned int create_program(unsigned int vertexShader, unsigned int fragmentShader);
    unsigned int create_shader(const char* code, GLenum type);

   public:
    Program(const char* vertexCode, const char* fragmentCode);
    ~Program();

    void set_int(const std::string name, int value) const;
    void set_float(const std::string name, float value) const;
    void set_vec3(const std::string name, glm::vec3 value) const;
    void set_vec4(const std::string name, glm::vec4 value) const;
    void set_matrix(const std::string name, glm::mat4 matrix) const;

    void bind() const;
};