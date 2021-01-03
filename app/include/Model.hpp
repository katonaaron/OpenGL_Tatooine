//
// Created by aron on 1/3/21.
//

#ifndef TATOOINE_MODEL_HPP
#define TATOOINE_MODEL_HPP

#include "Model3D.hpp"
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types


class Model : public gps::Model3D {
public:

    void updateModelMatrix(const glm::mat4& model, const glm::mat4& view);
    void updateNormalMatrix(const glm::mat4& view);
    const glm::mat4& getModelMatrix();
    const glm::mat3& getNormalMatrix();

    void init(gps::Shader& shaderProgram, const glm::mat4& model, const glm::mat4& view);
    void Draw(gps::Shader shaderProgram) override;

private:
    // matrices
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;

    // shader uniform locations
    GLuint modelLoc;
    GLuint normalMatrixLoc;

    void sendMatrices(gps::Shader& shaderProgram);
};


#endif //TATOOINE_MODEL_HPP
