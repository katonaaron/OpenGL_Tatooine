//
// Created by aron on 1/8/21.
//

#ifndef TATOOINE_LIGHTS_HPP
#define TATOOINE_LIGHTS_HPP

#include <glm/glm.hpp> //core glm functionality
#include <vector>
#include "Shader.hpp"

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

void sendDirLight(const DirLight &dirLight, gps::Shader shader);

void sendPointLights(const std::vector<PointLight> &pointLights, gps::Shader shader);

#endif //TATOOINE_LIGHTS_HPP
