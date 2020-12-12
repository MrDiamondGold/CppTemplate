#include "mesh.hpp"

Mesh::Mesh() {
    glGenVertexArrays(1, &this->VAO);

    update_mesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

void Mesh::update_mesh() {
    if (this->positions.size() < 1) {
        return;
    } else if (this->positions.size() != this->normals.size()) {
        throw new std::runtime_error("Mesh vertices size not the same as mesh normals size.");
    } else if (this->positions.size() != this->colors.size()) {
        throw new std::runtime_error("Mesh vertices size not the same as mesh colors size.");
    } else if (this->positions.size() != this->uvs.size()) {
        throw new std::runtime_error("Mesh vertices size not the same as mesh uvs size.");
    }

    if(this->VBO != NULL) {
        glDeleteBuffers(1, &this->VBO);
        glDeleteBuffers(1, &this->EBO);
    }

    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    
    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    size_t vertex_size = (sizeof(glm::vec3) * 3) + sizeof(glm::vec2);

    glBufferData(GL_ARRAY_BUFFER, vertex_size * this->positions.size(), NULL, GL_DYNAMIC_DRAW);

    void* data[] = {
        &this->positions.front(),
        &this->normals.front(),
        &this->colors.front(),
        &this->uvs.front()
    };

    size_t vertex_sizes[] = {
        sizeof(glm::vec3),
        sizeof(glm::vec3),
        sizeof(glm::vec3),
        sizeof(glm::vec2),
    };

    size_t lengths[] = {
        this->positions.size(),
        this->normals.size(),
        this->colors.size(),
        this->uvs.size()
    };

    size_t offset = 0;
    for(int i = 0; i < 4; i++) {
        size_t vertex_size = vertex_sizes[i];
        size_t size = vertex_size * lengths[i];

        glEnableVertexAttribArray(i);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data[i]);
        glVertexAttribPointer(i, vertex_size / sizeof(float), GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);

        offset += size;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * this->indices.size(), &this->indices.front(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
}