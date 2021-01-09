//
// Created by aron on 1/6/21.
//

#ifndef TATOOINE_VIEW_MODE_HPP
#define TATOOINE_VIEW_MODE_HPP

#define GLEW_STATIC

#include <GL/glew.h>
#include "Shader.hpp"

#define UNIFORM_POLYGONAL_VIEW "polygonalView"

enum class ViewMode {
    WIREFRAME,
    SOLID_SMOOTH,
    SOLID_POLYGONAL
};

void setViewMode(gps::Shader shader, ViewMode viewMode);

void nextViewMode(gps::Shader shader, ViewMode &viewMode);

#endif //TATOOINE_VIEW_MODE_HPP
