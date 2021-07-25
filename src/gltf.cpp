#include "gltf.h"

#include "glad/glad.h"
#include "json.h"
#include "utility.h"

using namespace nlohmann;

namespace gltf {
gltf_info* load_from_path(const char* path) {
    std::filesystem::path fs_path = std::filesystem::path(path);
    if (fs_path.extension().string() != ".gltf") {
        std::stringstream ss;
        ss << "Unsupported file type, expected *.gltf not " << fs_path.extension() << '\n';

        throw std::runtime_error(ss.str());
    }

    json j = json::parse(file_to_string(path));
    gltf_info* info = new gltf_info();

    gltf_json gltf = gltf_json();

    gltf.scene = std::optional<size_t>();
    gltf.scenes = std::vector<gltf_scene>();
    gltf.nodes = std::vector<gltf_node>();
    gltf.meshes = std::vector<gltf_mesh>();
    gltf.cameras = std::vector<gltf_camera>();
    gltf.accessors = std::vector<gltf_accessor>();
    gltf.buffers = std::vector<gltf_buffer>();
    gltf.buffer_views = std::vector<gltf_buffer_view>();
    gltf.materials = std::vector<gltf_material>();
    gltf.textures = std::vector<gltf_texture>();
    gltf.images = std::vector<gltf_image>();
    gltf.samplers = std::vector<gltf_sampler>();

    try {
        for (const auto& [key, value] : j.items()) {
            if (key == "scene") {
                gltf.scene = value;
            } else if (key == "scenes") {
                gltf.scenes.reserve(value.size());
                for (const auto& s : value) {
                    const auto& nodes = s.at("nodes");

                    gltf_scene scene = gltf_scene();
                    scene.nodes = std::vector<size_t>();
                    scene.nodes.reserve(nodes.size());
                    for (const auto& n : nodes) {
                        scene.nodes.push_back(n);
                    }
                    gltf.scenes.push_back(scene);
                }
            } else if (key == "nodes") {
                gltf.nodes.reserve(value.size());
                for (const auto& n : value) {
                    gltf_node node = gltf_node();
                    node.children = std::vector<size_t>();
                    node.camera = std::nullopt;
                    node.mesh = std::nullopt;
                    node.name = std::nullopt;

                    node.translation = std::nullopt;
                    node.rotation = std::nullopt;
                    node.scale = std::nullopt;

                    for (const auto& [n_key, n_value] : n.items()) {
                        if (n_key == "children") {
                            node.children.reserve(n_value.size());
                            for (const auto& c : n_value) {
                                node.children.push_back(c);
                            }
                        } else if (n_key == "camera") {
                            node.camera = n_value;
                        } else if (n_key == "mesh") {
                            node.mesh = n_value;
                        } else if (n_key == "name") {
                            node.name = n_value;
                        } else if (n_key == "translation") {
                            glm::vec3 translation;
                            translation.x = n_value[0];
                            translation.y = n_value[1];
                            translation.z = n_value[2];
                            node.translation = translation;
                        } else if (n_key == "rotation") {
                            glm::quat rotation;
                            rotation.x = n_value[0];
                            rotation.y = n_value[1];
                            rotation.z = n_value[2];
                            rotation.w = n_value[3];
                            node.rotation = rotation;
                        } else if (n_key == "scale") {
                            glm::vec3 scale;
                            scale.x = n_value[0];
                            scale.y = n_value[1];
                            scale.z = n_value[2];
                            node.scale = scale;
                        }
                    }

                    gltf.nodes.push_back(node);
                }
            } else if (key == "meshes") {
                gltf.meshes.reserve(value.size());

                for (const auto& m : value) {
                    gltf_mesh mesh = gltf_mesh();
                    mesh.primitives = std::vector<gltf_primitive>();

                    for (const auto& [m_key, m_value] : m.items()) {
                        if (m_key == "primitives") {
                            mesh.primitives.reserve(m_value.size());
                            for (const auto& p : m_value) {
                                gltf_primitive primitive = gltf_primitive();
                                primitive.attributes = std::vector<std::pair<std::string, size_t>>();
                                primitive.indices = p.at("indices");
                                primitive.material = p.at("material");
                                primitive.mode = p.value("mode", GL_TRIANGLES);

                                const auto& attributes = p.at("attributes");
                                primitive.attributes.reserve(attributes.size());

                                for (const auto& [a_key, a_value] : attributes.items()) {
                                    primitive.attributes.push_back(std::pair(a_key, a_value));
                                }

                                mesh.primitives.push_back(primitive);
                            }
                        }
                    }

                    gltf.meshes.push_back(mesh);
                }
            } else if (key == "cameras") {
            } else if (key == "accessors") {
                gltf.accessors.reserve(value.size());

                for (const auto& a : value) {
                    gltf_accessor accessor = gltf_accessor();
                    accessor.max = std::nullopt;
                    accessor.min = std::nullopt;
                    accessor.buffer_view = a.at("bufferView");
                    accessor.component_type = a.at("componentType");
                    accessor.count = a.at("count");
                    accessor.type = a.at("type");
                    accessor.byte_offset = a.value("byteOffset", 0);

                    const auto& max = a.value("max", std::vector<size_t>());
                    const auto& min = a.value("min", std::vector<size_t>());

                    if (max.size() > 0) {
                        accessor.max = std::vector<std::variant<size_t, float>>();
                        accessor.max.value().reserve(max.size());
                        for (const auto& m : max) {
                            accessor.max.value().push_back(m);
                        }
                    }
                    if (min.size() > 0) {
                        accessor.min = std::vector<std::variant<size_t, float>>();
                        accessor.min.value().reserve(max.size());
                        for (const auto& m : min) {
                            accessor.min.value().push_back(m);
                        }
                    }

                    gltf.accessors.push_back(accessor);
                }
            } else if (key == "bufferViews") {
                gltf.buffer_views.reserve(value.size());

                for (const auto& v : value) {
                    gltf_buffer_view buffer_view = gltf_buffer_view();
                    buffer_view.buffer = v.at("buffer");
                    buffer_view.byte_offset = v.value("byteOffset", 0);
                    buffer_view.byte_length = v.value("byteLength", 0);
                    buffer_view.byte_stride = v.value("byteStride", std::optional<size_t>());
                    buffer_view.target = v.value("target", std::optional<size_t>());

                    gltf.buffer_views.push_back(buffer_view);
                }
            } else if (key == "buffers") {
                gltf.buffers.reserve(value.size());

                for (const auto& b : value) {
                    gltf_buffer buffer = gltf_buffer();
                    buffer.uri = b.at("uri");
                    buffer.byte_length = b.at("byteLength");

                    gltf.buffers.push_back(buffer);
                }
            } else if (key == "materials") {
                gltf.materials.reserve(value.size());

                for (const auto& m : value) {
                    gltf_material material = gltf_material();

                    material.metallic_roughness.base_color_factor = {1, 1, 1, 1};
                    material.metallic_roughness.metallic_factor = 0.5;
                    material.metallic_roughness.roughness_factor = 0.5;
                    material.metallic_roughness.base_color_texture = std::nullopt;
                    material.metallic_roughness.metallic_roughness_texture = std::nullopt;

                    material.emissive_factor = {1, 1, 1};
                    material.alpha_mode = "OPAQUE";
                    material.normal_texture = std::nullopt;
                    material.occlusion_texture = std::nullopt;
                    material.emissive_texture = std::nullopt;

                    for (const auto& [m_key, m_value] : m.items()) {
                        if (m_key == "pbrMetallicRoughness") {
                            for (const auto& [p_key, p_value] : m_value.items()) {
                                if (p_key == "baseColorTexture") {
                                    material.metallic_roughness.base_color_texture = {
                                        p_value.at("index"),
                                        (size_t)p_value.value("texCoord", 0)};
                                } else if (p_key == "metallicRoughnessTexture") {
                                    material.metallic_roughness.metallic_roughness_texture = {
                                        p_value.at("index"),
                                        (size_t)p_value.value("texCoord", 0)};
                                } else if (p_key == "baseColorFactor") {
                                    material.metallic_roughness.base_color_factor = {p_value[0], p_value[1], p_value[2], p_value[3]};
                                } else if (p_key == "metallicFactor") {
                                    material.metallic_roughness.metallic_factor = p_value;
                                } else if (p_key == "roughnessFactor") {
                                    material.metallic_roughness.roughness_factor = p_value;
                                } else if (p_key == "alphaMode") {
                                    material.alpha_mode = p_value;
                                }
                            }
                        } else if (m_key == "normalTexture") {
                            gltf_normal_texture normal_texture = gltf_normal_texture();

                            normal_texture.index = m_value.at("index");
                            normal_texture.tex_coord = m_value.value("texCoord", 0);
                            normal_texture.scale = m_value.value("scale", 1.0f);

                            material.normal_texture = normal_texture;
                        } else if (m_key == "occlusionTexture") {
                            gltf_occlusion_texture occlusion_texture = gltf_occlusion_texture();

                            occlusion_texture.index = m_value.at("index");
                            occlusion_texture.tex_coord = m_value.value("texCoord", 0);
                            occlusion_texture.strength = m_value.value("strength", 1.0f);

                            material.occlusion_texture = occlusion_texture;
                        } else if (m_key == "emissiveTexture") {
                            gltf_emissive_texture emissive_texture = gltf_emissive_texture();

                            emissive_texture.index = m_value.at("index");
                            emissive_texture.tex_coord = m_value.value("texCoord", 0);

                            material.emissive_texture = emissive_texture;
                        } else if (m_key == "emissiveFactor") {
                            material.emissive_factor = {m_value[0], m_value[1], m_value[2]};
                        }
                    }

                    gltf.materials.push_back(material);
                }
            } else if (key == "textures") {
                gltf.textures.reserve(value.size());

                for (const auto& t : value) {
                    gltf_texture texture = gltf_texture();
                    texture.sampler = t.value("sampler", std::optional<size_t>());
                    if (t.find("extensions") != t.end()) {
                        const auto& e = t.at("extensions");
                        if (e.find("KHR_texture_basisu") != e.end()) {
                            texture.source = e.at("KHR_texture_basisu").at("source");
                        }
                    } else {
                        texture.source = t.at("source");
                    }

                    gltf.textures.push_back(texture);
                }
            } else if (key == "images") {
                gltf.images.reserve(value.size());

                for (const auto& i : value) {
                    gltf_image image = gltf_image();

                    std::string uri = i.at("uri");
                    image.image = uri;

                    gltf.images.push_back(image);
                }
            } else if (key == "samplers") {
                gltf.samplers.reserve(value.size());

                for (const auto& s : value) {
                    gltf_sampler sampler = gltf_sampler();

                    sampler.mag_filter = s.value("magFilter", GL_REPEAT);
                    sampler.min_filter = s.value("minFilter", GL_REPEAT);
                    sampler.wrap_s = s.value("wrapS", GL_LINEAR);
                    sampler.wrap_t = s.value("wrapT", GL_LINEAR);

                    gltf.samplers.push_back(sampler);
                }
            } else {
            }
        }

        size_t total_bytes = 0;
        for (const auto& buffer : gltf.buffers) {
            total_bytes += buffer.byte_length;
        }

        char* data = new char[total_bytes];

        size_t offset = 0;
        for (const auto& buffer : gltf.buffers) {
            std::filesystem::path directory = std::filesystem::path(path).parent_path();
            std::filesystem::path path = directory.append(buffer.uri);

            std::ifstream binary(path.string(), std::ios::in | std::ios::binary);
            binary.read(data + offset, buffer.byte_length);
            binary.close();

            offset += buffer.byte_length;
        }

        info->json = gltf;
        info->data = data;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        throw e;
    }

    return info;
}

void destroy(gltf_info* info) {
    if (info->data != NULL) {
        delete[] info->data;
    }
    if (info != NULL) {
        delete info;
    }
}
}  // namespace gltf
