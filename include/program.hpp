#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>

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

    void set_matrix(const std::string name, glm::mat4 matrix) const;

    void bind() const;
};