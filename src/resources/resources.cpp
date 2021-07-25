#include "resources.h"

Resources* Resources::_singleton;

GLTFScene Resources::load_gltf(std::filesystem::path path) {
    GLTFScene scene = *GLTFScene::from_file(path.string());
    return scene;
}

Texture Resources::load_image(std::filesystem::path path) {
    Texture texture = Texture::from_file(path.string());
    return texture;
}

Shader Resources::load_shader(std::filesystem::path path) {
    Shader shader = {file_to_string(path.string())};
    return shader;
}

Resources* Resources::get_singleton() {
    if (_singleton == nullptr) {
        _singleton = new Resources();
    }

    return _singleton;
}

Resources::Resources()
    : resources(std::unordered_map<std::type_index, std::any>()) {
    resources.insert(std::pair(std::type_index(typeid(Shader)), std::unordered_map<std::string, Shader>()));
    resources.insert(std::pair(std::type_index(typeid(Texture)), std::unordered_map<std::string, Texture>()));
    resources.insert(std::pair(std::type_index(typeid(GLTFScene)), std::unordered_map<std::string, GLTFScene>()));

    /*
    const std::string path = "assets";

    const std::type_info& shader_type = typeid(Shader);
    // const std::type_info& texture_type = typeid(Texture);
    // const std::type_info& primitive_type = typeid(Primitive);

    auto shaders = std::unordered_map<std::string, Shader>();
    // auto textures = std::map<std::string, Texture>();
    // auto primitives = std::map<std::string, Primitive>();

    auto shader_files = std::vector<std::filesystem::path>();
    // auto image_files = std::vector<std::filesystem::path>();
    // auto texture_files = std::vector<std::filesystem::path>();
    // auto gltf_files = std::vector<std::filesystem::path>();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        auto path = entry.path();
        if (path.has_extension()) {
            auto extension = path.extension();
            if (extension == ".glsl") {
                shader_files.push_back(path);
            }
            // else if (extension == ".png") {
            //     continue;
            //     image_files.push_back(path);
            // } else if (extension == ".ktx") {
            //     continue;
            //     texture_files.push_back(path);
            // } else if (extension == ".gltf") {
            //     gltf_files.push_back(path);
            // }
        }
    }

    for (const auto& file : shader_files) {
        shaders.insert(std::pair(file.filename().string(), Shader::from_file(file.string())));
    }

    for (const auto& file : image_files) {
        textures.insert(std::pair(file.filename().string(), load_image(file)));
    }

    for (const auto& file : texture_files) {
        unsigned int id = load_ktx_texture(file.string());
        auto texture = Texture{id, 0, 0, 0};
        textures.insert(std::pair(file.filename().string(), texture));
    }

    for (const auto& file : gltf_files) {
        
    }
    */
}

Resources::~Resources() {
}