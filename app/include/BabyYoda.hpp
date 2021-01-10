//
// Created by aron on 1/10/21.
//

#ifndef TATOOINE_BABYYODA_HPP
#define TATOOINE_BABYYODA_HPP

#include "Model.hpp"

#include <GLFW/glfw3.h>


class BabyYoda : public Model {
public:
    void Draw(gps::Shader shaderProgram) override;

    void startAnimation(float animationSpeed);

    void stopAnimation();

private:
    float speed = 0.0f;
    double lastTimeStamp = 0.0;
    float progress = 0.0f;
    bool isAnimated = false;

    const int rockMeshId = 1;
    const float rotationUnit = 15.0f;
    const float translationUnit = 0.1f;

    glm::mat4 computeRockTransformationMatrix();
};


#endif //TATOOINE_BABYYODA_HPP
