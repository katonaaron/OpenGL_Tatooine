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

void Model::init(const glm::mat4 &model, const glm::mat4 &view) {
    setModelMatrix(model);
    updateNormalMatrix(view);
}

void Model::init(const glm::mat4 &model) {
    init(model, glm::mat3(0.0f));
}

BoundingBox Model::getBoundingBox() const {
    return modelTransformBoundingBox(Model3D::getBoundingBox());
}

std::vector<BoundingBox> Model::getMeshBoundingBoxes() const {
    const std::vector<BoundingBox> &bbOrig = Model3D::getMeshBoundingBoxes();
    std::vector<BoundingBox> bbTranslated;
    bbTranslated.reserve(bbOrig.size());
    std::transform(bbOrig.begin(), bbOrig.end(), back_inserter(bbTranslated), [this](const BoundingBox &bb) {
        return modelTransformBoundingBox(bb);
    });
    return bbTranslated;
}

BoundingBox Model::modelTransformBoundingBox(const BoundingBox &boundingBox) const {
    return {
            .min = modelMatrix * boundingBox.min,
            .max = modelMatrix * boundingBox.max
    };
}
