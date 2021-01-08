//
// Created by aron on 1/3/21.
//

#include "Model.hpp"

void Model::setModelMatrix(const glm::mat4 &model) {
    modelMatrix = model;
    updateNormalMatrix(viewMatrix);
}

const glm::mat4 &Model::getModelMatrix() {
    return modelMatrix;
}

const glm::mat3 &Model::getNormalMatrix() {
    return normalMatrix;
}

void Model::updateNormalMatrix(const glm::mat4 &view) {
    viewMatrix = view;
    normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMatrix));
}

void Model::init(const glm::mat4 &model, const glm::mat4& view) {
    setModelMatrix(model);
    updateNormalMatrix(view);
}

void Model::init(const glm::mat4 &model) {
    init(model, glm::mat3(0.0f));
}
