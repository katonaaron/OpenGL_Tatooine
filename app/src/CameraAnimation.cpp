//
// Created by aron on 1/10/21.
//

#include "CameraAnimation.hpp"

#include <utility>

CameraAnimation::CameraAnimation(float speed, float alpha, std::vector<glm::vec3> controlPoints, gps::Camera &camera)
        : speed(speed), alpha(alpha), controlPoints(std::move(controlPoints)), camera(camera) {}

void CameraAnimation::animate() {
    // initialization on first animation
    if (!started) {
        started = true;
        lastTimeStamp = glfwGetTime();
        prevPosition = controlPoints.at(prev(currentPoint));
    }

    // compute the seconds since the last animation
    double currentTimeStamp = glfwGetTime();
    double elapsedSeconds = currentTimeStamp - lastTimeStamp;
    lastTimeStamp = currentTimeStamp;

    // compute the parameter of the Catmull–Rom spline
    currentT = glm::min(currentT + elapsedSeconds * speed, 1.0);

    // define the control points
    const glm::vec3 &p0 = controlPoints.at(prev(currentPoint));
    const glm::vec3 &p1 = controlPoints.at(currentPoint);
    const glm::vec3 &p2 = controlPoints.at(next(currentPoint));
    const glm::vec3 &p3 = controlPoints.at(next(next(currentPoint)));
    const glm::vec3 &p4 = controlPoints.at(next(next(next(currentPoint))));

    // obtain the next position in the Catmull–Rom spline
    glm::vec3 cameraPosition = CatmullRom(p0, p1, p2, p3, currentT, alpha);
    // preserve camera up direction
    prevPosition.y = cameraPosition.y;
    // compute camera target = point on the line defined by the previous and current positions
    glm::vec3 cameraTarget = (cameraPosition - prevPosition) + cameraPosition;
    // save camera position
    prevPosition = cameraPosition;

    // move camera
    camera.move(cameraPosition, cameraTarget);

    // if the control point is reached, move to the next point
    if (currentT > 0.9999f) {
        currentT = 0;
        currentPoint = next(currentPoint);
    }
}

void CameraAnimation::reset() {
    currentT = 0.0f;
    currentPoint = 0.0f;
    started = false;
}

int CameraAnimation::next(int pointNr) {
    return (pointNr + 1) % controlPoints.size();
}

int CameraAnimation::prev(int pointNr) {
    return (pointNr - 1 + controlPoints.size()) % controlPoints.size();
}

// source: https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline#Code_example_in_Unreal_C++
glm::vec3 CameraAnimation::CatmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                                      const glm::vec3 &p2, const glm::vec3 &p3,
                                      float t, float alpha) {
    float t0 = 0.0f;
    float t1 = getT(t0, alpha, p0, p1);
    float t2 = getT(t1, alpha, p1, p2);
    float t3 = getT(t2, alpha, p2, p3);

    t = glm::mix(t1, t2, t);

    glm::vec3 A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    glm::vec3 A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
    glm::vec3 A3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;
    glm::vec3 B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
    glm::vec3 B2 = (t3 - t) / (t3 - t1) * A2 + (t - t1) / (t3 - t1) * A3;
    glm::vec3 C = (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;

    return C;
}

// source: https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline#Code_example_in_Unreal_C++
float CameraAnimation::getT(float t, float alpha, const glm::vec3 &p0, const glm::vec3 &p1) {
    glm::vec3 dir = p1 - p0;
    float delta = glm::pow(glm::length(dir), alpha * 0.5);
    return t + delta;
}
