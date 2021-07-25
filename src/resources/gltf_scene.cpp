#include "gltf_scene.h"

size_t gltf_type_size(const std::string_view type) {
    if (type == "SCALAR") {
        return 1;
    } else if (type == "VEC2") {
        return 2;
    } else if (type == "VEC3") {
        return 3;
    } else if (type == "VEC4") {
        return 4;
    } else if (type == "MAT2") {
        return 4;
    } else if (type == "MAT3") {
        return 9;
    } else if (type == "MAT4") {
        return 16;
    } else {
        return 1;
    }
}

size_t gltf_component_size(const size_t& type) {
    switch (type) {
        case GL_BYTE:
            return 1;
        case GL_UNSIGNED_BYTE:
            return 1;
        case GL_SHORT:
            return 2;
        case GL_UNSIGNED_SHORT:
            return 2;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_FLOAT:
            return 4;
        default:
            return 1;
    }
}

Material load_material(std::filesystem::path folder, gltf::gltf_material gltf_material, gltf::gltf_info* info) {
    Material material = Material();
    material.albedo_factor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material.emissive_factor = glm::vec3(1.0f, 1.0f, 1.0f);
    material.normal_scale = 1.0f;
    material.occlusion_strength = 1.0f;
    material.alpha_type = AlphaType::OPAQUE;

    material.albedo_factor.r = gltf_material.metallic_roughness.base_color_factor.r;
    material.albedo_factor.g = gltf_material.metallic_roughness.base_color_factor.g;
    material.albedo_factor.b = gltf_material.metallic_roughness.base_color_factor.b;
    material.albedo_factor.a = gltf_material.metallic_roughness.base_color_factor.a;

    material.emissive_factor.r = gltf_material.emissive_factor.r;
    material.emissive_factor.g = gltf_material.emissive_factor.g;
    material.emissive_factor.b = gltf_material.emissive_factor.b;

    material.metallic_factor = gltf_material.metallic_roughness.metallic_factor;
    material.roughness_factor = gltf_material.metallic_roughness.roughness_factor;

    if (gltf_material.alpha_mode == "OPAQUE") {
        material.alpha_type = AlphaType::OPAQUE;
    } else if(gltf_material.alpha_mode == "MASK") {
        material.alpha_type = AlphaType::MASK;
    } else if(gltf_material.alpha_mode == "BLEND") {
        material.alpha_type = AlphaType::BLEND;
    }

    if (gltf_material.metallic_roughness.base_color_texture.has_value()) {
        size_t index = gltf_material.metallic_roughness.base_color_texture.value().index;
        auto gltf_texture = info->json.textures[index];
        auto gltf_source = info->json.images[gltf_texture.source];

        auto filename = std::get<std::string>(gltf_source.image);
        auto filename_path = std::filesystem::path(filename);
        auto path = folder;
        path.append(filename);
        material.albedo = Texture::from_file(path.string());
    } else {
        material.albedo = Texture::from_color(0xFFFFFFFF);
    }

    if(gltf_material.metallic_roughness.metallic_roughness_texture.has_value()) {
        size_t index = gltf_material.metallic_roughness.metallic_roughness_texture.value().index;
        auto gltf_texture = info->json.textures[index];
        auto gltf_source = info->json.images[gltf_texture.source];

        auto filename = std::get<std::string>(gltf_source.image);
        auto filename_path = std::filesystem::path(filename);
        auto path = folder;
        path.append(filename);
        material.metallic_roughness = Texture::from_file(path.string());
    } else {
        material.metallic_roughness = Texture::from_color(0xFFFFFFFF);
    }
    
    if(gltf_material.normal_texture.has_value()) {
        gltf::gltf_normal_texture gltf_normal_texture = gltf_material.normal_texture.value();
        size_t index = gltf_normal_texture.index;
        auto gltf_texture = info->json.textures[index];
        auto gltf_source = info->json.images[gltf_texture.source];

        auto filename = std::get<std::string>(gltf_source.image);
        auto filename_path = std::filesystem::path(filename);
        auto path = folder;
        path.append(filename);
        material.normal = Texture::from_file(path.string());
        material.normal_scale = gltf_normal_texture.scale;
    } else {
        material.normal = Texture::from_color(0xFFFF0000);
        material.normal_scale = 0.0f;
    }

    if(gltf_material.occlusion_texture.has_value()) {
        gltf::gltf_occlusion_texture gltf_occlusion_texture = gltf_material.occlusion_texture.value();
        size_t index = gltf_occlusion_texture.index;
        auto gltf_texture = info->json.textures[index];
        auto gltf_source = info->json.images[gltf_texture.source];

        auto filename = std::get<std::string>(gltf_source.image);
        auto filename_path = std::filesystem::path(filename);
        auto path = folder;
        path.append(filename);
        material.occlusion = Texture::from_file(path.string());
        material.occlusion_strength = gltf_occlusion_texture.strength;
    } else {
        material.occlusion = Texture::from_color(0xFFFFFFFF);
    }

    return material;
}

Mesh load_mesh(size_t index, gltf::gltf_info* info, std::filesystem::path folder) {
    Mesh mesh = Mesh();
    mesh.primitives = std::vector<Primitive>();

    gltf::gltf_mesh m = info->json.meshes.at(index);
    size_t p_i = 0;
    for (const auto& p : m.primitives) {
        Primitive prim = Primitive();
        prim.material = load_material(folder, info->json.materials.at(p.material), info);

        glGenVertexArrays(1, &prim.VAO);
        glGenBuffers(1, &prim.VBO);
        glGenBuffers(1, &prim.EBO);

        glm::vec3* positions = nullptr;
        size_t positions_size = 0;
        glm::vec3* normals = nullptr;
        size_t normals_size = 0;
        size_t normals_count = 0;
        glm::vec4* tangents = nullptr;
        size_t tangents_size = 0;
        glm::vec2* uvs = nullptr;
        size_t uvs_size = 0;

        size_t buffer_size = 0;
        for (const auto& a : p.attributes) {
            gltf::gltf_accessor accessor = info->json.accessors[a.second];
            gltf::gltf_buffer_view buffer_view = info->json.buffer_views[accessor.buffer_view];
            size_t component_size = gltf_type_size(accessor.type) * gltf_component_size(accessor.component_type);
            void* source = &info->data[accessor.byte_offset + buffer_view.byte_offset];
            size_t source_size = component_size * accessor.count;

            if (a.first == "POSITION") {
                positions = new glm::vec3[accessor.count];
                memcpy(positions, source, source_size);
                positions_size = source_size;
            } else if (a.first == "NORMAL") {
                normals = new glm::vec3[accessor.count];
                memcpy(normals, source, source_size);
                normals_size = source_size;
                normals_count = accessor.count;
            } else if (a.first == "TANGENT") {
                tangents = new glm::vec4[accessor.count];
                memcpy(tangents, source, source_size);
                tangents_size = source_size;
            } else if (a.first == "TEXCOORD_0") {
                uvs = new glm::vec2[accessor.count];
                memcpy(uvs, source, source_size);
                uvs_size = source_size;
            }
        }

        // if(tangents_size == 0) {
        //     throw std::runtime_error("Primitive has no tangent data.");
        // }

        void* ptrs[] = {
            positions,
            normals,
            tangents,
            uvs};

        size_t sizes[] = {
            positions_size,
            normals_size,
            tangents_size,
            uvs_size};

        size_t vertex_sizes[] = {
            sizeof(glm::vec3),
            sizeof(glm::vec3),
            sizeof(glm::vec4),
            sizeof(glm::vec2)};

        glBindVertexArray(prim.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, prim.VBO);
        glBufferData(GL_ARRAY_BUFFER, positions_size + normals_size + tangents_size + uvs_size, NULL, GL_DYNAMIC_DRAW);

        size_t offset = 0;
        for (int i = 0; i < 4; i++) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, sizes[i], ptrs[i]);
            glEnableVertexAttribArray(i);

            glVertexAttribPointer(i, vertex_sizes[i] / sizeof(float), GL_FLOAT, GL_FALSE, vertex_sizes[i], (void*)offset);
            offset += sizes[i];
        }

        delete[] positions;
        delete[] normals;
        delete[] tangents;
        delete[] uvs;

        gltf::gltf_accessor accessor = info->json.accessors[p.indices];
        gltf::gltf_buffer_view buffer_view = info->json.buffer_views[accessor.buffer_view];

        prim.size = accessor.count;
        prim.EBO_type = accessor.component_type;
        prim.mode = p.mode;

        void* data = &info->data[accessor.byte_offset + buffer_view.byte_offset];
        size_t component_size = gltf_type_size(accessor.type.c_str()) * gltf_component_size(accessor.component_type);
        size_t data_size = accessor.count * component_size;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mesh.primitives.push_back(prim);

        p_i++;
    }
    return mesh;
}

Camera load_camera(size_t index, gltf::gltf_info* info) {
    return Camera();
}

Node load_node(gltf::gltf_node gltf_node, GLTFScene* scene, gltf::gltf_info* info, std::filesystem::path folder) {
    Node node = Node();
    node.camera = -1;
    node.mesh = -1;
    node.name = gltf_node.name.value_or("");
    node.children = std::vector<Node>();
    node.transform = glm::mat4(1.0);

    if(gltf_node.scale.has_value()) {
        node.transform = glm::scale(node.transform, gltf_node.scale.value());
    }
    if(gltf_node.rotation.has_value()) {
        node.transform = node.transform * glm::mat4_cast(gltf_node.rotation.value());
    }
    if(gltf_node.translation.has_value()) {
        node.transform = glm::translate(node.transform, gltf_node.translation.value());
    }

    if (gltf_node.camera.has_value()) {
        node.camera = scene->cameras.size();
        scene->cameras.push_back(load_camera(gltf_node.camera.value(), info));
    }
    if (gltf_node.mesh.has_value()) {
        node.mesh = scene->meshes.size();
        scene->meshes.push_back(load_mesh(gltf_node.mesh.value(), info, folder));
    }
    for (const auto& cn : gltf_node.children) {
        gltf::gltf_node gltf_child_node = info->json.nodes.at(cn);
        node.children.push_back(load_node(gltf_child_node, scene, info, folder));
    }
    return node;
}

GLTFScene* GLTFScene::from_file(const std::string path) {
    GLTFScene* scene = new GLTFScene();
    scene->cameras = std::vector<Camera>();
    scene->meshes = std::vector<Mesh>();
    scene->nodes = std::vector<Node>();
    scene->path = path;

    gltf::gltf_info* info = gltf::load_from_path(path.c_str());

    auto fs_path = std::filesystem::path(path);
    auto folder = fs_path.parent_path();

    if (info->json.scene.has_value()) {
        gltf::gltf_scene gltf_scene = info->json.scenes.at(info->json.scene.value());

        for (const auto& rn : gltf_scene.nodes) {
            gltf::gltf_node gltf_node = info->json.nodes.at(rn);
            scene->nodes.push_back(load_node(gltf_node, scene, info, folder));
        }
    }

    gltf::destroy(info);

    return scene;
}