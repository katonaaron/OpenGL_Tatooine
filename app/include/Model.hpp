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

    void setModelMatrix(const glm::mat4 &model);

    void updateNormalMatrix(const glm::mat4 &view);

    const glm::mat4 &getModelMatrix();

    const glm::mat3 &getNormalMatrix();

    void init(const glm::mat4 &model, const glm::mat4 &view);

    void init(const glm::mat4 &model);

private:
    // matrices
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;
    glm::mat3 viewMatrix;
};


#endif //TATOOINE_MODEL_HPP
