#pragma once

#include <any>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "glad/glad.h"
#include "gltf.h"
#include "gltf_scene.h"
#include "material.h"
#include "mesh.h"
#include "program.h"
#include "shader.h"
#include "texture.h"
#include "utility.h"

class Resources {
   private:
    static Resources* _singleton;

    std::unordered_map<std::type_index, std::any> resources;

    GLTFScene load_gltf(std::filesystem::path path);
    Texture load_image(std::filesystem::path path);
    Shader load_shader(std::filesystem::path path);

   public:
    static Resources* get_singleton();

    Resources();
    ~Resources();

    std::any load_resource(const std::string_view path) {
        auto fs_path = std::filesystem::path(path);
        if (fs_path.has_extension()) {
            auto fs_extension = fs_path.extension();
            auto extension = fs_extension.string();

            if (extension == ".gltf") {
                return load_gltf(fs_path);
            } else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".ktx") {
                return load_image(fs_path);
            } else if (extension == ".glsl" || extension == ".shader") {
                return load_shader(fs_path);
            }
        } else {
            auto ss = std::stringstream();
            ss << "Resource has no extension, cannot infer type: " << path;
            throw std::runtime_error(ss.str());
        }
        return nullptr;
    }
};