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
