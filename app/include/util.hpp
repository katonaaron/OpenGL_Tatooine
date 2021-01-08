//
// Created by aron on 1/8/21.
//

#ifndef TATOOINE_UTIL_HPP
#define TATOOINE_UTIL_HPP

#include <glm/glm.hpp> //core glm functionality

glm::vec3 getPerpendicularTo(const glm::vec3 &vec);

void print(const glm::vec3& vec);

void println(const glm::vec3& vec);

float constrain(float x, float a, float b);

#endif //TATOOINE_UTIL_HPP
