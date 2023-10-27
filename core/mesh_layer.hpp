#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "vertices_layer.hpp"

namespace Ez3DGL{

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Texture {
    unsigned int id;
    enum class Type {
        Diffuse,
        Specula
    };
};

class Mesh{
public:
    
};

/**
 * @brief 光源对象,支持定向光源,点光源,聚光
 * 
 */
class Light{
public:

    light_dir_t dir_light;
    // note that the size has limited
    std::vector<light_point_t> point_lights;
    light_spot_t spot_light;


};

}

