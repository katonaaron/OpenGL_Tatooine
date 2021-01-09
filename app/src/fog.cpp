//
// Created by aron on 1/9/21.
//

#include "fog.hpp"

void sendFog(const Fog &fog, gps::Shader shader) {
    shader.setUniform("fog.position", fog.position);
    shader.setUniform("fog.color", fog.color);
    shader.setUniform("fog.density", fog.density);
}
