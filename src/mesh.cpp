#include "mesh.hpp"

Mesh::Mesh() {
    glGenVertexArrays(1, &this->VAO);

    updateMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::updateMesh() {
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    
    glBindVertexArray(this->VAO);

    glm::vec3 *vertexFront = NULL;
    if(vertices.size() > 0)
    {
        vertexFront = &vertices.front();
    }

    unsigned int *indexFront = NULL;
    if(indices.size() > 0)
    {
        indexFront = &indices.front();
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertices.size(), vertexFront, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->indices.size(), indexFront, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
}