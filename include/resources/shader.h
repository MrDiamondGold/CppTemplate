#pragma once

#include <string>

#include "utility.h"

struct Shader {
    std::string data;

    static Shader from_file(const std::string file);
};