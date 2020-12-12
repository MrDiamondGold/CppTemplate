#pragma once

#include "glad/glad.h"

#include <filesystem>
#include <typeindex>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <istream>
#include <ranges>
#include <string>
#include <chrono>
#include <ktx.h>
#include <map>
#include <any>

#include "program.hpp"
#include "utility.hpp"


struct Shader { // Temp
    std::string data;
};

struct Texture { // Temp
    unsigned int id;
    int width, height, channels;
};

class Resources {
   private:
    static Resources* _singleton;

    std::map<std::type_index, std::any> resources;
   public:
    static Resources& get_singleton();
    
    Resources();
    ~Resources();

    template<class T>
    std::map<std::string, T>& get_resource() {
        return std::any_cast<std::map<std::string, T>&>(resources.at(std::type_index(typeid(T))));
    }

};