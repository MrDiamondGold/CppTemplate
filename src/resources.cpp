#include "resources.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #define LOG_TIME(path) std::cout << path << ": " << (std::chrono::high_resolution_clock::now() - begin).count() / 1000000000.0 << '\n';

Resources* Resources::_singleton;

unsigned int gen_gl_texture(int channels, int width, int height, unsigned char* raw) {
    unsigned int id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum type;
    switch(channels) {
        case 4:
            type = GL_RGBA;
            break;
        case 3:
            type = GL_RGB;
            break;
        case 2:
            type = GL_RG;
            break;
        case 1:
            type = GL_RED;
            break;
        default:
            throw std::runtime_error("Invalid channel count.");
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, raw);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

Resources::Resources()
    : resources(std::map<std::type_index, std::any>()) {
    const std::string path = "assets";

    const std::type_info& shader_type = typeid(Shader);
    const std::type_info& texture_type = typeid(Texture);

    auto shaders = std::map<std::string, Shader>();
    auto textures = std::map<std::string, Texture>();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        auto entry_path = entry.path();
        if (entry_path.has_extension()) {
            if (entry_path.extension() == ".glsl") {
                auto contents = file_to_string(entry_path.string());
                auto shader = Shader { contents };

                shaders.insert(std::pair(entry_path.filename().string(), shader));
            } else if (entry_path.extension() == ".png") {
                int width, height, channels;

                auto raw = stbi_load(entry_path.string().c_str(), &width, &height, &channels, 0);
                if (raw) {
                    unsigned int id = gen_gl_texture(channels, width, height, raw);

                    stbi_image_free(raw);

                    auto texture = Texture { id, width, height, channels };

                    textures.insert(std::pair(entry_path.filename().string(), texture));
                } else {
                    std::stringstream error_stream;
                    error_stream << "Failed to load texture: " << entry_path << ".\n";

                    throw std::runtime_error(error_stream.str());
                }
            } else if (entry_path.extension() == ".ktx") {
                ktxTexture* kTexture;
                KTX_error_code result;
                ktx_size_t offset;
                ktx_uint8_t* image;
                ktx_uint32_t level, layer, face_slice;
                GLuint id = 0;
                GLenum target, gl_error;

                result = ktxTexture_CreateFromNamedFile(entry_path.string().c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);
                glGenTextures(1, &id);
                result = ktxTexture_GLUpload(kTexture, &id, &target, &gl_error);

                auto texture = Texture { id, (int)kTexture->baseWidth, (int)kTexture->baseHeight, 0 };
                ktxTexture_Destroy(kTexture);

                textures.insert(std::pair(entry_path.filename().string(), texture));
            }
        }
    }

    resources.insert(std::pair(std::type_index(shader_type), shaders));
    resources.insert(std::pair(std::type_index(texture_type), textures));
}

Resources::~Resources() {
}

Resources& Resources::get_singleton() {
    if (_singleton == nullptr) {
        _singleton = new Resources();
    }

    return *_singleton;
}