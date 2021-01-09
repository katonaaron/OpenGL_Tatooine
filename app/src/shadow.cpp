//
// Created by aron on 1/9/21.
//

#include "shadow.hpp"

void initShadowMapFBO(GLsizei shadowWidth, GLsizei shadowHeight, GLuint &shadowMapFBO, GLuint &depthMapTexture) {
    //Create the FBO, the depth texture and attach the depth texture to the FBO

    // generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 shadowWidth,
                 shadowHeight,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix(const Sun& sun) {
    //Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(sun.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 0.1f, 0.0f));

    const GLfloat near_plane = 200.0f, far_plane = 600.0f, sides = 100.f;
    glm::mat4 lightProjection = glm::ortho(-sides, sides, -sides, sides - 10, near_plane, far_plane);

    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}