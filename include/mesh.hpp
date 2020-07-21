#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Mesh {
   protected:
    unsigned int VAO, VBO, EBO;

   public:
     std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
     std::vector<glm::vec3> normals = std::vector<glm::vec3>();
     std::vector<glm::vec3> colors = std::vector<glm::vec3>();
     std::vector<glm::vec2> uvs = std::vector<glm::vec2>();
     std::vector<uint32_t> indices = std::vector<uint32_t>();

     Mesh();
     ~Mesh();

     void updateMesh();
     void draw() const;
};