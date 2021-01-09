//
// Created by aron on 1/8/21.
//

#ifndef TATOOINE_SUN_HPP
#define TATOOINE_SUN_HPP

#include "Model3D.hpp"
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "util.hpp"

class Sun : public gps::Model3D {
public:

    void init(const glm::vec3 &rotationAxis, GLfloat radius, GLfloat scaleFactor, GLfloat angle);

    const glm::vec3 &getRotationAxis() const;

    void setRotationAxis(const glm::vec3 &rotationAxis);

    GLfloat getRadius() const;

    void setRadius(GLfloat radius);

    GLfloat getScale() const;

    void setScale(GLfloat scaleFactor);

    void scale(GLfloat scaleFactor);

    GLfloat getAngle() const;

    void setAngle(GLfloat angle);

    void rotate(GLfloat angle);

    const glm::vec3 getPosition() const;

    void Draw(gps::Shader shaderProgram) override;

private:
    glm::vec3 rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 perpendicularAxis = glm::vec3(0.0f, 1.0f, 0.0f);;
    GLfloat radius = 0.0f;
    GLfloat scaleFactor = 1.0f;
    GLfloat angle = 0.0f;

    // matrices
    glm::mat4 rotationMat = glm::mat4(1.0f);
    glm::mat4 translationMat = glm::mat4(1.0f);
    glm::mat4 scaleMat = glm::mat4(1.0f);
    glm::mat4 modelMat = glm::mat4(1.0f);

    void _setRotationAxis(const glm::vec3 &rotationAxis);

    void _setRadius(GLfloat radius);

    void _setScale(GLfloat scaleFactor);

    void _setAngle(GLfloat angle);

    void updateRotationMatrix();

    void updateTranslationMatrix();

    void updateScaleMatrix();

    void updateModelMatrix();
};


#endif //TATOOINE_SUN_HPP
