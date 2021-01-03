//
// Created by aron on 1/3/21.
//

#include "Model.hpp"

void Model::Draw(gps::Shader shaderProgram) {
    shaderProgram.useShaderProgram();

    sendMatrices(shaderProgram);
    Model3D::Draw(shaderProgram);
}

void Model::updateModelMatrix(const glm::mat4 &model, const glm::mat4 &view) {
    modelMatrix = model;
    updateNormalMatrix(view);
}

const glm::mat4 &Model::getModelMatrix() {
    return modelMatrix;
}

const glm::mat3 &Model::getNormalMatrix() {
    return normalMatrix;
}

void Model::updateNormalMatrix(const glm::mat4 &view) {
    normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMatrix));
}

void Model::init(gps::Shader& shaderProgram, const glm::mat4 &model, const glm::mat4& view) {
    shaderProgram.useShaderProgram();

    modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");
    normalMatrixLoc = glGetUniformLocation(shaderProgram.shaderProgram, "normalMatrix");

    updateModelMatrix(model, view);
    sendMatrices(shaderProgram);
}

void Model::sendMatrices(gps::Shader &shaderProgram) {
    //send base scene model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    //send base scene normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}
