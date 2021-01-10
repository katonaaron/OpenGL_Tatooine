//
// Created by aron on 1/10/21.
//

#ifndef TATOOINE_CAMERAANIMATION_HPP
#define TATOOINE_CAMERAANIMATION_HPP

#include <glm/glm.hpp> //core glm functionality
#include <GLFW/glfw3.h>
#include <vector>
#include "Camera.hpp"

class CameraAnimation {
public:
    // constants for alpha
    static constexpr float UNIFORM = 0.0f;
    static constexpr float CENTRIPETAL = 0.5f;
    static constexpr float CHORDAL = 0.1f;

    CameraAnimation(float speed, float alpha, std::vector<glm::vec3> controlPoints, gps::Camera &camera);

    // moves the camera to the next position
    void animate();

    // resets the animation to the initial state
    void reset();

private:
    std::vector<glm::vec3> controlPoints;
    gps::Camera &camera;
    float speed;
    float alpha;

    float currentT = 0.0f;
    int currentPoint = 0;
    bool started = false;

    double lastTimeStamp = 0.0;
    glm::vec3 prevPosition;

    int next(int pointNr);

    int prev(int pointNr);

    glm::vec3 CatmullRom(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, float t,
                         float alpha);

    float getT(float t, float alpha, const glm::vec3 &p0, const glm::vec3 &p1);
};


#endif //TATOOINE_CAMERAANIMATION_HPP
