#include "state.h"

void APIENTRY on_gl_error(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* user_params) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void State::init_glfw() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    this->window = glfwCreateWindow(800, 600, "Temp", NULL, NULL);
    // window = glfwCreateWindow(arguments.width, arguments.height, title.c_str(), NULL, NULL);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create glfw window.");
    }

    glfwSetWindowSizeLimits(this->window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(this->window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load OpenGL context.");
    }
    glfwMaximizeWindow(this->window);

    glEnable(GL_MULTISAMPLE);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(on_gl_error, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // glfwSwapInterval(0);
    // glfwSetWindowSizeCallback(window, on_window_resize);
}

void State::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    this->io = &ImGui::GetIO();
    this->io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    (void)this->io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void State::deinit_glfw() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void State::deinit_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

State::State(int argc, char** argv)
    : scene(nullptr), total_textures(0), completed_textures(0), ktx(false) {
    CLI::App app{title}; // TODO: Move command line argument parsing out of state.

    unsigned int width = 1280, height = 720;

    CLI::App* convert = app.add_subcommand("convert", "Converts a glTF file to an engine usable format.")->ignore_case();

    app.add_option("--width", width, "Window width.");
    app.add_option("--height", height, "Window height.");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        exit(app.exit(e));
    }

    if (app.got_subcommand(convert)) {
        exit(0);
    } else {
        init_glfw();
        init_imgui();

        glViewport(0, 0, width, height);
    }
}

State::~State() {
    deinit_imgui();
    deinit_glfw();
}

void State::pre_render() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void State::render() {
    glfwSwapBuffers(this->window);
    glfwPollEvents();
}

void State::update() {
    if(this->total_textures > 0) {
        if (this->completed_textures == this->total_textures) {
            for(auto& thread : this->toktx_threads) {
                thread.join();
                std::cout << "Joined." << '\n';
            }
            this->total_textures = 0;
        }
    }
}

void State::open_gltf_file_dialog() {
    NFD::UniquePath in_path;
    nfdfilteritem_t filters[1] = {{"glTF", "gltf"}};
    nfdresult_t result = NFD::OpenDialog(in_path, filters, 1);
    if (result == NFD_OKAY) {
        auto new_scene = GLTFScene::from_file(in_path.get());
        if(scene != nullptr) {
            delete scene;
        }
        scene = new_scene;
    } else if (result == NFD_CANCEL) {
    } else {
        throw std::runtime_error(NFD::GetError());
    }
}

void State::save_gltf_file_dialog() {
    NFD::UniquePath out_path;
    nfdfilteritem_t filters[1] = {{"glTF", "gltf"}};
    nfdresult_t result = NFD::SaveDialog(out_path, filters, 1);
    if (result == NFD_OKAY) {
        if (scene != nullptr) {
            if (this->ktx) {
                auto path = std::filesystem::path(out_path.get());
                path = path.parent_path();
                for (const auto& file : std::filesystem::directory_iterator("temp")) {
                    auto file_path = path;
                    std::filesystem::copy_file(file.path(), file_path.append(file.path().filename().string()), std::filesystem::copy_options::overwrite_existing);
                }
                std::filesystem::remove_all("temp");
            }
        }
    } else if (result == NFD_CANCEL) {
    } else {
        throw std::runtime_error(NFD::GetError());
    }
}

void State::convert_current_file() {
    if(this->scene != nullptr) {
        gltf::gltf_info* info = gltf::load_from_path(this->scene->path.c_str()); // TODO: Don't load the entire .bin file when the json is all that is needed.
        
        auto fs_path = std::filesystem::path(this->scene->path);
        auto folder = fs_path.parent_path();

        std::vector<std::filesystem::path> images = std::vector<std::filesystem::path>();
        images.reserve(info->json.images.size());
        for (const auto& gltf_image : info->json.images) {
            std::string image = std::get<std::string>(gltf_image.image);
            auto path = folder;
            path = path.append(image);
            images.push_back(path);
        }

        gltf::destroy(info);

        ktx = true;
        std::filesystem::create_directory("temp");

        total_textures = images.size();
        completed_textures = 0;
        this->toktx_threads = std::vector<std::thread>();
        for(const auto& image : images) {
            std::stringstream ss;
            ss << "toktx --genmipmap --zcmp 22 " << "temp/" << image.filename().string() << ".ktx " << image.string();
            const std::string command = ss.str();

            this->toktx_threads.push_back(std::thread(&State::convert_texture, this, command));
        }
    }
}

void State::convert_texture(const std::string command) {
    std::system(command.c_str());
    this->completed_textures++;
    std::cout << this->completed_textures << '\n';
}