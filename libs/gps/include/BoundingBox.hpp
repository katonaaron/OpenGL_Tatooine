//
// Created by aron on 1/10/21.
//

#ifndef TATOOINE_BOUNDINGBOX_HPP
#define TATOOINE_BOUNDINGBOX_HPP

#include <glm/glm.hpp> //core glm functionality

struct BoundingBox {
    glm::vec4 min;
    glm::vec4 max;
};

bool collides(const BoundingBox &bb1, const BoundingBox &bb2);

#endif //TATOOINE_BOUNDINGBOX_HPP
