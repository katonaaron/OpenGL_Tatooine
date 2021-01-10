//
// Created by aron on 1/8/21.
//

#include <iostream>
#include "util.hpp"

glm::vec3 getPerpendicularTo(const glm::vec3 &vec) {
    glm::vec3 e;
    if (vec.y == 0 && vec.z == 0) {
        e = glm::vec3(0.0f, 1.0f, 0.0f);
    } else {
        e = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    return glm::normalize(glm::cross(e, vec));
}

void print(const glm::vec3 &vec) {
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

void print(const glm::mat4 &mat) {
    std::cout << "(\n";
    for (int i = 0; i < 4; i++) {
        std::cout << " (";
        for (int j = 0; j < 4; j++) {
            if (j > 0)
                std::cout << ", ";
            std::cout << mat[j][i];
        }
        std::cout << ")\n";
    }
    std::cout << ")\n";
}

void println(const glm::vec3 &vec) {
    print(vec);
    std::cout << "\n";
}

void println(const glm::mat4 &mat) {
    print(mat);
    std::cout << "\n";
}

float constrain(float x, float a, float b) {
    if (x < a)
        return a;
    if (b < x)
        return b;
    return x;
}

float angleBetween(const glm::vec3 &vec1, const glm::vec3 &vec2) {
    return glm::degrees(glm::acos(glm::dot(vec1, vec2)));
}
