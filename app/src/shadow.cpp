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
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
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

glm::mat4 computeLightSpaceTrMatrix(const glm::vec3 &lightDir, const BoundingBox &sceneBoundingBox) {
    // alias
    const glm::vec4 &bbMin = sceneBoundingBox.min;
    const glm::vec4 &bbMax = sceneBoundingBox.max;

    // the camera position will be the point from unit distance from the origin in the direction of the light
    const glm::vec3 &lightPos = lightDir;

    // compute the camera up direction
    glm::vec3 cameraUp;
    if (lightPos.y == 0) {
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    } else {
        float deltaY = glm::length(glm::vec3(lightPos.x, 0.0f, lightPos.z)) / lightPos.y;
        cameraUp = glm::vec3(-lightPos.x, deltaY, -lightPos.z);
        if (lightPos.y < 0) {
            cameraUp *= -1;
        }
        cameraUp = glm::normalize(cameraUp);
    }

    // create
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), cameraUp);

    // obtain all vertices of the bounding box of the scene in the coordinate space of the light
    glm::vec3 c1 = lightView * bbMin;
    glm::vec3 c2 = lightView * glm::vec4(bbMin.x, bbMin.y, bbMax.z, 1.0f);
    glm::vec3 c3 = lightView * glm::vec4(bbMin.x, bbMax.y, bbMin.z, 1.0f);
    glm::vec3 c4 = lightView * glm::vec4(bbMin.x, bbMax.y, bbMax.z, 1.0f);
    glm::vec3 c5 = lightView * glm::vec4(bbMax.x, bbMin.y, bbMin.z, 1.0f);
    glm::vec3 c6 = lightView * glm::vec4(bbMax.x, bbMin.y, bbMax.z, 1.0f);
    glm::vec3 c7 = lightView * glm::vec4(bbMax.x, bbMax.y, bbMin.z, 1.0f);
    glm::vec3 c8 = lightView * bbMax;

    // invert the z values
    c1.z *= -1;
    c2.z *= -1;
    c3.z *= -1;
    c4.z *= -1;
    c5.z *= -1;
    c6.z *= -1;
    c7.z *= -1;
    c8.z *= -1;

    // obtain the extreme values of the scene
    glm::vec3 minLight = glm::min(c1, glm::min(c2, glm::min(c3, glm::min(c4, glm::min(c5, glm::min(c6, glm::min(c7,
                                                                                                                c8)))))));
    glm::vec3 maxLight = glm::max(c1, glm::max(c2, glm::max(c3, glm::max(c4, glm::max(c5, glm::max(c6, glm::max(c7,
                                                                                                                c8)))))));

    // create projection which zooms maximally to the scene
    glm::mat4 lightProjection = glm::ortho(
            minLight.x,
            maxLight.x,
            minLight.y,
            maxLight.y,
            minLight.z,
            maxLight.z
    );

    // compute the light space transformation matrix
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}