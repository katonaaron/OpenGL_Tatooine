//
// Created by aron on 1/8/21.
//

#include "Sun.hpp"

void Sun::init(const glm::vec3 &rotationAxis, GLfloat radius, GLfloat scaleFactor, GLfloat angle) {
    _setRotationAxis(rotationAxis);
    _setRadius(radius);
    _setScale(scaleFactor);
    _setAngle(angle);
    updateRotationMatrix();
    updateTranslationMatrix();
    updateScaleMatrix();
    updateModelMatrix();
}

const glm::vec3 &Sun::getRotationAxis() const {
    return rotationAxis;
}

void Sun::setRotationAxis(const glm::vec3 &rotationAxis) {
    _setRotationAxis(rotationAxis);
    updateRotationMatrix();
    updateTranslationMatrix();
    updateModelMatrix();
}

GLfloat Sun::getRadius() const {
    return radius;
}

void Sun::setRadius(GLfloat radius) {
    _setRadius(radius);
    updateTranslationMatrix();
    updateModelMatrix();
}

GLfloat Sun::getScale() const {
    return scaleFactor;
}

void Sun::setScale(GLfloat scaleFactor) {
    _setScale(scaleFactor);
    updateScaleMatrix();
    updateModelMatrix();
}

void Sun::scale(GLfloat scaleFactor) {
    setScale(Sun::scaleFactor + scaleFactor);
}

GLfloat Sun::getAngle() const {
    return angle;
}

void Sun::setAngle(GLfloat angle) {
    _setAngle(angle);
    updateRotationMatrix();
    updateModelMatrix();
}

void Sun::rotate(GLfloat angle) {
    setAngle(Sun::angle + angle);
}

void Sun::updateRotationMatrix() {
    rotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), rotationAxis);
}

void Sun::updateTranslationMatrix() {
    translationMat = glm::translate(glm::identity<glm::mat4>(), perpendicularAxis * radius);
}

void Sun::updateScaleMatrix() {
    scaleMat = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
}

void Sun::updateModelMatrix() {
    modelMat = rotationMat * translationMat * scaleMat;
}

void Sun::Draw(gps::Shader shaderProgram) {
    shaderProgram.useShaderProgram();
    shaderProgram.setUniform("model", modelMat);

    Model3D::Draw(shaderProgram);
}

const glm::vec3 Sun::getPosition() const {
    return glm::vec3(rotationMat * translationMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Sun::_setRotationAxis(const glm::vec3 &axis) {
    rotationAxis = glm::normalize(axis);
    perpendicularAxis = getPerpendicularTo(rotationAxis);
}

void Sun::_setRadius(GLfloat radius) {
    Sun::radius = radius;
}

void Sun::_setScale(GLfloat scaleFactor) {
    Sun::scaleFactor = scaleFactor;
}

void Sun::_setAngle(GLfloat angle) {
    Sun::angle = angle;
}

