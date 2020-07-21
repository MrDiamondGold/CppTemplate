#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Mesh {
   protected:
    unsigned int VAO, VBO, EBO;

   public:
     std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
     std::vector<unsigned int> indices = std::vector<unsigned int>();

     Mesh();
     ~Mesh();

     void updateMesh();
     void draw() const;
};