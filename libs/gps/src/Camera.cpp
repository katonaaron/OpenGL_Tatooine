#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraUpDirection = cameraUp;
        move(cameraPosition, cameraTarget);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    //consider only the x, z coordinates in order to preserve height
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        cameraPosition = nextPosition(direction, speed);
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::move(glm::vec3 cameraPosition, glm::vec3 cameraTarget) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        // Constrain angle
        static float angleX = 0.0f;
        pitch = glm::clamp(angleX + pitch, -89.9f, 89.9f) - angleX;
        angleX = angleX + pitch;

        glm::mat4 rot(1.0f);

        // rotate around "y"
        rot = glm::rotate(rot, glm::radians(yaw), cameraUpDirection);
        // rotate around "x"
        rot = glm::rotate(rot, glm::radians(pitch), cameraRightDirection);

        // update axis and target
        cameraFrontDirection = rot * glm::vec4(cameraFrontDirection, 1.0f);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        cameraTarget = cameraPosition + cameraFrontDirection;

        // use this or don't use this and limit the angles
        //cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    glm::vec3 Camera::nextPosition(MOVE_DIRECTION direction, float speed) {
        glm::vec3 frontDir = glm::normalize(glm::vec3(cameraFrontDirection.x, 0.0f, cameraFrontDirection.z));
        glm::vec3 rightDir = glm::normalize(glm::vec3(cameraRightDirection.x, 0.0f, cameraRightDirection.z));

        switch (direction) {
            case MOVE_FORWARD:
                return cameraPosition + frontDir * speed;
            case MOVE_BACKWARD:
                return cameraPosition - frontDir * speed;
            case MOVE_RIGHT:
                return cameraPosition + rightDir * speed;
            case MOVE_LEFT:
                return cameraPosition - rightDir * speed;
        }
    }
}