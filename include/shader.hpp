#pragma once

#include <glad/glad.h>
#include <vector>

class Shader {
   protected:
    unsigned int id;

   private:
    unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
    unsigned int createShader(const char* code, GLenum type);

   public:
    Shader(const char* vertexCode, const char* fragmentCode);
    ~Shader();

    void bind() const;
};