//
// Created by aron on 1/10/21.
//

#include <util.hpp>
#include "BabyYoda.hpp"

void BabyYoda::Draw(gps::Shader shaderProgram) {
    int i = 0;
    for (auto &mesh : getMeshes()) {
        if (isAnimated && i == rockMeshId) {
            shaderProgram.useShaderProgram();
            shaderProgram.setUniform("model", computeRockTransformationMatrix());
        }

        mesh.Draw(shaderProgram);

        if (isAnimated && i == rockMeshId) {
            shaderProgram.useShaderProgram();
            shaderProgram.setUniform("model", getModelMatrix());
        }

        i++;
    }
}

void BabyYoda::startAnimation(float animationSpeed) {
    isAnimated = true;
    lastTimeStamp = glfwGetTime();
    BabyYoda::speed = animationSpeed;
}

void BabyYoda::stopAnimation() {
    isAnimated = false;
}

glm::mat4 BabyYoda::computeRockTransformationMatrix() {
    // compute the seconds since the last animation
    double currentTimeStamp = glfwGetTime();
    float elapsedSeconds = currentTimeStamp - lastTimeStamp;
    lastTimeStamp = currentTimeStamp;

    // increment the progress
    progress += speed * elapsedSeconds;

    // clamp and reverse the progress
    if (progress >= 1.0f) {
        progress = 1.0f;
        speed *= -1;
    } else if (progress <= -1.0f) {
        progress = -1.0f;
        speed *= -1;
    }

    // rotation angle
    float angle = progress * rotationUnit;
    // y axis translation value
    float displacement = progress * translationUnit;

    BoundingBox bb = getBoundingBox();
    // center of the object
    glm::vec3 center = (bb.max + bb.min) * 0.5f;

    glm::mat4 model = glm::mat4(1.0f);
    // rotate around the center of the object
    model = glm::translate(model, center);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, -center);
    // translate vertically
    model = glm::translate(model, glm::vec3(0.0f, displacement, 0.0f));
    // apply the object transformations
    model = model * getModelMatrix();

    return model;
}
