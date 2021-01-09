//
// Created by aron on 1/9/21.
//

#ifndef TATOOINE_SHADOW_HPP
#define TATOOINE_SHADOW_HPP

#include <glm/glm.hpp> //core glm functionality
#include <GL/glew.h>

#include "Sun.hpp"

void initShadowMapFBO(GLsizei shadowWidth, GLsizei shadowHeight, GLuint &shadowMapFBO, GLuint &depthMapTexture);

glm::mat4 computeLightSpaceTrMatrix(const Sun& sun);


#endif //TATOOINE_SHADOW_HPP
