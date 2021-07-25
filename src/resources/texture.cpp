#include "texture.h"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture Texture::from_color(uint32_t color) {
    return from_memory(1, 1, 4, (uint8_t*)&color);
}

Texture Texture::from_file(const std::string file) {
    auto fs_file = std::filesystem::path(file);

    int32_t width, height, channels;
    uint8_t* data = stbi_load(file.c_str(), &width, &height, &channels, 0);
    Texture texture = from_memory(width, height, channels, data);
    stbi_image_free(data);
    
    return texture;
}

Texture Texture::from_memory(uint32_t width, uint32_t height, uint32_t channels, uint8_t* data) {
    Texture texture;

    texture.width = width;
    texture.height = height;
    texture.channels = channels;

    glGenTextures(1, &texture.id);

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum type;
    switch (channels) {
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

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

// unsigned int load_hdr(const std::string path) {
//     stbi_set_flip_vertically_on_load(true);
//     int width, height, channels;
//     float *data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
//     unsigned int id;
//     if (data) {
//         glGenTextures(1, &id);
//         glBindTexture(GL_TEXTURE_2D, id);
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     } else {
//         throw std::runtime_error("Couldn't load data.");
//     }
// }