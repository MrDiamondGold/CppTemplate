#pragma once

#include <glm/glm.hpp>

#include "texture.h"

enum AlphaType {
    OPAQUE,
    MASK,
    BLEND,
};

struct Material {
    Texture albedo;
    Texture normal;
    Texture occlusion;
    Texture metallic_roughness;

    glm::vec4 albedo_factor;
    glm::vec3 emissive_factor;
    float metallic_factor;
    float roughness_factor;
    float normal_scale;
    float occlusion_strength;
    AlphaType alpha_type;
};