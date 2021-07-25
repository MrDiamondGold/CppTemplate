#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "gltf.h"
#include "gltf_scene.h"
#include "material.h"
#include "mesh.h"
#include "program.h"
#include "resources.h"
#include "state.h"
#include "texture.h"
#include "utility.h"

#define global_static static

global_static size_t news;
global_static size_t deletes;

void* operator new(size_t size) {
    news++;
    return malloc(size);
}

void operator delete(void* p) {
    free(p);
    deletes++;
}

void render_node(Node node, GLTFScene* scene, Program* program, glm::mat4 transform, bool (*predicate)(Primitive)) {
    program->set_matrix("u_model", transform);
    program->set_vec3("u_light.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
    program->set_vec3("u_light.color", glm::vec3(1.0f, 1.0f, 1.0f) * 2.5f);

    if (node.mesh != -1) {
        const auto& mesh = scene->meshes.at(node.mesh);
        for (const auto& primitive : mesh.primitives) {
            if ((predicate)(primitive)) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, primitive.material.albedo.id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, primitive.material.normal.id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, primitive.material.metallic_roughness.id);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, primitive.material.occlusion.id);

                program->set_int("s_albedo", 0);
                program->set_int("s_normal", 1);
                program->set_int("s_metallic", 2);
                program->set_int("s_roughness", 2);
                program->set_int("s_ao", 3);

                program->set_vec4("u_albedo", primitive.material.albedo_factor);
                program->set_float("u_normal", primitive.material.normal_scale);
                program->set_float("u_metallic", primitive.material.metallic_factor);
                program->set_float("u_roughness", primitive.material.roughness_factor);
                program->set_float("u_ao", primitive.material.occlusion_strength);

                glBindVertexArray(primitive.VAO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.EBO);
                glDrawElements(primitive.mode, primitive.size, primitive.EBO_type, 0);
                glBindVertexArray(0);
            }
        }
    }

    for (const auto& child_node : node.children) {
        render_node(child_node, scene, program, transform * child_node.transform, predicate);
    }
}

void draw_scene(Node node, GLTFScene* scene) {
    if (ImGui::TreeNode(node.name.c_str())) {
        ImGui::Indent(8);
        for (const auto& child : node.children) {
            draw_scene(child, scene);
        }
        ImGui::Unindent(8);
        ImGui::TreePop();
    }
}

int main(int argc, char** argv) {
    NFD::Guard gaurd = NFD::Guard();
    State state = State(argc, argv);

    auto* resources = Resources::get_singleton();

    auto vertex_shader = std::any_cast<Shader>(resources->load_resource("assets/shaders/pbr.vert.glsl"));
    auto fragment_shader = std::any_cast<Shader>(resources->load_resource("assets/shaders/pbr.frag.glsl"));

    // auto vertex_shader = std::any_cast<Shader>(resources->load_resource("assets/shaders/hdr_mapping.vert.glsl"));
    // auto fragment_shader = std::any_cast<Shader>(resources->load_resource("assets/shaders/hdr_mapping.frag.glsl"));

    Program program = Program(vertex_shader.data.c_str(), fragment_shader.data.c_str());

    // std::system("toktx assets/grass_texture.ktx assets/grass_texture.png");

    auto last_time = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::high_resolution_clock::duration::zero();

    auto samples = 8;

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int colorbuffer;
    glGenTextures(1, &colorbuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, 800, 600, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorbuffer, 0);

    unsigned int renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete.");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int msaa_framebuffer;
    glGenFramebuffers(1, &msaa_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, msaa_framebuffer);

    unsigned int screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete.");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float pitch = -18.0f, yaw = -90.0f;
    glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
    glm::vec3 view_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    ImVec2 last_delta = ImVec2(0, 0);
    int width, height;
    glfwGetWindowSize(state.window, &width, &height);
    bool about = false;
    while (!glfwWindowShouldClose(state.window)) {
        glfwPollEvents();

        float camera_speed = 2.0f * delta.count() / 1000000000;
        if (glfwGetKey(state.window, GLFW_KEY_W)) {
            position += camera_speed * view_direction;
        }
        if (glfwGetKey(state.window, GLFW_KEY_A)) {
            position -= camera_speed * glm::normalize(glm::cross(view_direction, glm::vec3(0.0f, 1.0, 0.0f)));
        }
        if (glfwGetKey(state.window, GLFW_KEY_S)) {
            position -= camera_speed * view_direction;
        }
        if (glfwGetKey(state.window, GLFW_KEY_D)) {
            position += camera_speed * glm::normalize(glm::cross(view_direction, glm::vec3(0.0f, 1.0, 0.0f)));
        }
        if (glfwGetKey(state.window, GLFW_KEY_SPACE)) {
            position.y += camera_speed;
        }
        if (glfwGetKey(state.window, GLFW_KEY_LEFT_SHIFT)) {
            position.y -= camera_speed;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Load")) {
                    state.open_gltf_file_dialog();
                }
                if (ImGui::MenuItem("Save")) {
                    state.save_gltf_file_dialog();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About", nullptr)) {
                    about = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (about) {
            ImGui::OpenPopup("About");
        }

        ImGui::SetNextWindowSize(ImVec2(256, 128), about);
        ImGui::SetNextWindowPos(ImVec2(width / 2.0f - 128.0f, height / 2.0f - 64.0f), about);
        if (ImGui::BeginPopupModal("About", &about, ImGuiWindowFlags_NoResize)) {
            ImGui::TextWrapped("Version: %d.%d.%d", 0, 0, 1);
            ImGui::TextWrapped("Description: glTF file converter for converting to internally usable format.");
            ImGui::EndPopup();
        }

        bool tree = true;
        if (ImGui::Begin("Tree"), &tree) {
            if (state.scene != nullptr) {
                for (const auto& node : state.scene->nodes) {
                    draw_scene(node, state.scene);
                }
            }
            ImGui::End();
        }

        bool convert = true;
        if (ImGui::Begin("Convert", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            float width = ImGui::GetWindowContentRegionWidth();

            if (ImGui::Button("Convert", ImVec2(width, 0))) {
                if (state.total_textures == 0) {
                    state.convert_current_file();
                }
            }
            float progress = 0.0f;
            if (state.total_textures > 0) {
                progress = (float)state.completed_textures / (float)state.total_textures;
            }
            ImGui::ProgressBar(progress);

            ImGui::End();
        }

        if (ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoCollapse)) {
            ImGui::BeginChild("Render");
            ImGui::Text("News: %u", news);
            ImGui::SameLine();
            ImGui::Text("Deletes: %u", deletes);
            ImGui::SameLine();
            ImGui::Text("Difference: %i", (int)news - (int)deletes);
            if (ImGui::IsMouseDragging(0) && ImGui::IsWindowFocused()) {
                ImVec2 delta = ImGui::GetMouseDragDelta(0);
                ImVec2 motion = delta;
                motion.x -= last_delta.x;
                motion.y -= last_delta.y;
                last_delta = delta;
                pitch -= motion.y * 0.15f;
                yaw += motion.x * 0.15f;
            } else {
                last_delta = ImVec2(0, 0);
            }

            ImVec2 size = ImGui::GetWindowSize();
            GLsizei render_width = (GLsizei)size.x;
            GLsizei render_height = (GLsizei)size.y;

            glm::vec3 dir;
            dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            dir.y = sin(glm::radians(pitch));
            dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            view_direction = glm::normalize(dir);

            glViewport(0, 0, render_width, render_height);
            auto projection_matrix = glm::perspective(glm::radians(45.0f), (float)render_width / (float)render_height, 0.01f, 100.0f);
            auto view_matrix = glm::lookAt(position, position + view_direction, glm::vec3(0.0f, 1.0f, 0.0f));

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, render_width, render_height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, render_width, render_height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, msaa_framebuffer);
            glBindTexture(GL_TEXTURE_2D, screen_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_width, render_height, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glCullFace(GL_BACK);

            program.bind();
            program.set_vec3("u_view_pos", position);
            program.set_matrix("u_view", view_matrix);
            program.set_matrix("u_projection", projection_matrix);
            if (state.scene != nullptr) {
                for (const auto& node : state.scene->nodes) {
                    render_node(node, state.scene, &program, node.transform, [](Primitive primitive) {
                        return primitive.material.alpha_type == AlphaType::OPAQUE;
                    });
                }
            }

            if (state.scene != nullptr) {
                for (const auto& node : state.scene->nodes) {
                    render_node(node, state.scene, &program, node.transform, [](Primitive primitive) {
                        return primitive.material.alpha_type != AlphaType::OPAQUE;
                    });
                }
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, msaa_framebuffer);
            glBlitFramebuffer(0, 0, render_width, render_height, 0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            ImGui::Image((ImTextureID)screen_texture, size, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
            ImGui::End();
        }
        ImGui::ShowMetricsWindow(nullptr);
        ImGui::Render();

        // Normal rendering

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (state.io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(state.window);

        state.update();

        auto now = std::chrono::high_resolution_clock::now();
        delta = now - last_time;
        last_time = now;
    }
    // NOTE: Redundant
    delete Resources::get_singleton();

    // NOTE: Due to similar cases to last note difference will probably never be 0
    std::cout << "News: " << news << '\n'
              << "Deletes: " << deletes << '\n'
              << "Difference: " << (int)news - (int)deletes << '\n';

    return 0;
}