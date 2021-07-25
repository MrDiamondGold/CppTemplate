#include "shader.h"

Shader Shader::from_file(const std::string file) {
    return {file_to_string(file)};
}