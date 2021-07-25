#pragma once

#include <filesystem>

#include "glad/glad.h"
#include "utility.h"

struct Texture {
    uint32_t id;
    uint32_t width, height, channels;

    static Texture from_color(uint32_t color);
    static Texture from_file(const std::string file);
    static Texture from_memory(uint32_t width, uint32_t height, uint32_t channels, uint8_t* data);
};

// unsigned int load_hdr(const std::string path);