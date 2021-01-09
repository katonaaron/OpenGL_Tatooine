//
// Created by aron on 1/9/21.
//

#ifndef TATOOINE_FOG_HPP
#define TATOOINE_FOG_HPP

#include <glm/glm.hpp> //core glm functionality
#include "Shader.hpp"

struct Fog {
    glm::vec3 position;
    glm::vec4 color;
    float density;
};

void sendFog(const Fog &fog, gps::Shader shader);

#endif //TATOOINE_FOG_HPP
