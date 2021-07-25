#pragma once

#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glad/glad.h"
#include "gltf.h"
#include "material.h"
#include "mesh.h"
#include "texture.h"

struct Node {
    std::string name;
    std::vector<Node> children;
    size_t camera;
    size_t mesh;
    glm::mat4 transform;
};

struct Camera {
    std::string type;
};

struct GLTFScene {
    std::string path;
    
    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
    std::vector<Camera> cameras;

    static GLTFScene* from_file(const std::string path);
};