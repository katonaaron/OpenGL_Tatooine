//
// Created by aron on 1/8/21.
//

#ifndef TATOOINE_UTIL_HPP
#define TATOOINE_UTIL_HPP

#include <glm/glm.hpp> //core glm functionality

glm::vec3 getPerpendicularTo(const glm::vec3 &vec);

void print(const glm::vec3& vec);

void print(const glm::mat4& mat);

void println(const glm::vec3& vec);

void println(const glm::mat4& mat);

float constrain(float x, float a, float b);

// vec1, vec2 must be normalized
// result in degrees
float angleBetween(const glm::vec3& vec1, const glm::vec3& vec2);

#endif //TATOOINE_UTIL_HPP
