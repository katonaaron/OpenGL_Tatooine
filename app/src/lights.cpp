//
// Created by aron on 1/8/21.
//

#include "lights.hpp"

void sendDirLight(const DirLight &dirLight, gps::Shader shader) {
    shader.setUniform("dirLight.direction", dirLight.direction);
    shader.setUniform("dirLight.ambient", dirLight.ambient);
    shader.setUniform("dirLight.diffuse", dirLight.diffuse);
    shader.setUniform("dirLight.specular", dirLight.specular);
}

void sendPointLights(const std::vector<PointLight> &pointLights, gps::Shader shader) {
    int i = 0;
    for (const auto &pointLight : pointLights) {
        std::string elem = "pointLights[" + std::to_string(i) + "]";
        shader.setUniform(elem + ".position", pointLight.position);
        shader.setUniform(elem + ".constant", pointLight.constant);
        shader.setUniform(elem + ".linear", pointLight.linear);
        shader.setUniform(elem + ".quadratic", pointLight.quadratic);
        shader.setUniform(elem + ".ambient", pointLight.ambient);
        shader.setUniform(elem + ".diffuse", pointLight.diffuse);
        shader.setUniform(elem + ".specular", pointLight.specular);
        i++;
    }
}
