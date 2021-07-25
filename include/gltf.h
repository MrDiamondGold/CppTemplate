#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace gltf {
struct gltf_scene {
    std::vector<size_t> nodes;
};

struct gltf_node {
    std::vector<size_t> children;
    std::optional<size_t> mesh;
    std::optional<size_t> camera;
    std::optional<std::string> name;

    std::optional<glm::vec3> translation;
    std::optional<glm::quat> rotation;
    std::optional<glm::vec3> scale;
};

struct gltf_primitive {
    unsigned int mode;
    size_t indices;
    std::vector<std::pair<std::string, size_t>> attributes;
    size_t material;
};

struct gltf_mesh {
    std::vector<gltf_primitive> primitives;
};

struct gltf_perspective_camera {
    float aspect_ratio;
    float y_fov;
    float z_far;
    float z_near;
};

struct gltf_orthographic_camera {
    float x_mag;
    float y_mag;
    float z_far;
    float z_near;
};

struct gltf_camera {
    std::variant<gltf_perspective_camera, gltf_orthographic_camera> camera;
};

struct gltf_accessor {
    size_t buffer_view;
    size_t byte_offset;
    std::string type;
    unsigned int component_type;
    size_t count;
    std::optional<std::vector<std::variant<size_t, float>>> min;
    std::optional<std::vector<std::variant<size_t, float>>> max;
};

struct gltf_buffer_view {
    size_t buffer;
    size_t byte_offset;
    size_t byte_length;
    std::optional<size_t> byte_stride;
    std::optional<size_t> target;
};

struct gltf_buffer {
    size_t byte_length;
    std::string uri;
};

struct gltf_pbr_texture {
    size_t index;
    size_t tex_coord;
};

struct gltf_normal_texture {
    float scale;
    size_t index;
    size_t tex_coord;
};

struct gltf_occlusion_texture {
    float strength;
    size_t index;
    size_t tex_coord;
};

struct gltf_emissive_texture {
    size_t index;
    size_t tex_coord;
};

struct gltf_rgba {
    float r, g, b, a;
};

struct gltf_rgb {
    float r, g, b;
};

struct gltf_pbr_metallic_roughness {
    std::optional<gltf_pbr_texture> base_color_texture;
    gltf_rgba base_color_factor;
    std::optional<gltf_pbr_texture> metallic_roughness_texture;
    float metallic_factor;
    float roughness_factor;
};

struct gltf_material {
    gltf_pbr_metallic_roughness metallic_roughness;
    std::optional<gltf_normal_texture> normal_texture;
    std::optional<gltf_occlusion_texture> occlusion_texture;
    std::optional<gltf_emissive_texture> emissive_texture;
    gltf_rgb emissive_factor;
    std::string alpha_mode;
};

struct gltf_texture {
    size_t source;
    std::optional<size_t> sampler;
};

struct gltf_image {
    std::variant<std::string, std::pair<size_t, std::string>> image;
};

struct gltf_sampler {
    unsigned int mag_filter;
    unsigned int min_filter;
    unsigned int wrap_s;
    unsigned int wrap_t;
};

struct gltf_json {
    std::optional<size_t> scene;
    std::vector<gltf_scene> scenes;
    std::vector<gltf_node> nodes;
    std::vector<gltf_camera> cameras;
    std::vector<gltf_mesh> meshes;
    std::vector<gltf_accessor> accessors;
    std::vector<gltf_buffer> buffers;
    std::vector<gltf_buffer_view> buffer_views;
    std::vector<gltf_material> materials;
    std::vector<gltf_texture> textures;
    std::vector<gltf_image> images;
    std::vector<gltf_sampler> samplers;
};

typedef char* gltf_data;

struct gltf_info {
    gltf_json json;
    gltf_data data;
};

gltf_info* load_from_path(const char* path);
void destroy(gltf_info* info);
}  // namespace gltf
