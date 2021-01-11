//
// Created by aron on 1/10/21.
//

#include "BoundingBox.hpp"

bool collides(const BoundingBox &bb1, const BoundingBox &bb2) {
    bool collisionX = bb1.max.x >= bb2.min.x && bb2.max.x >= bb1.min.x;
    bool collisionY = bb1.max.y >= bb2.min.y && bb2.max.y >= bb1.min.y;
    bool collisionZ = bb1.max.z >= bb2.min.z && bb2.max.z >= bb1.min.z;
    return collisionX && collisionY && collisionZ;
}
