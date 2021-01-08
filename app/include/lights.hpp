//
// Created by aron on 1/8/21.
//

#ifndef TATOOINE_LIGHTS_HPP
#define TATOOINE_LIGHTS_HPP

#include <glm/glm.hpp> //core glm functionality
#include "Shader.hpp"

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

void sendDirLight(const DirLight& dirLight, gps::Shader shader);

#endif //TATOOINE_LIGHTS_HPP
