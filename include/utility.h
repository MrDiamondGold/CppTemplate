#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline std::string file_to_string(const std::string_view path) {
    std::string string = {path.begin(), path.end()};
    auto file = std::ifstream(string);
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

inline std::vector<std::string> split(std::string string, const std::string delimitter) {
    auto ret = std::vector<std::string>();

    size_t pos = 0;
    std::string token;
    while ((pos = string.find(delimitter)) != std::string::npos) {
        token = string.substr(0, pos);
        ret.push_back(token);
        string.erase(0, pos + delimitter.length());
    }

    return ret;
}