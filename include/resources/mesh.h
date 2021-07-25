#pragma once

#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

#include "glad/glad.h"
#include "material.h"

struct Primitive {
    unsigned int VAO, VBO, EBO;
    size_t size;
    unsigned int EBO_type;
    unsigned int mode;
    Material material;
};

struct Mesh {
    std::vector<Primitive> primitives;
};