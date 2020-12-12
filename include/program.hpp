#pragma once

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

    void bind() const;
};