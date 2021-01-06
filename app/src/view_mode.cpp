//
// Created by aron on 1/6/21.
//

#include "view_mode.hpp"

void setViewMode(gps::Shader shader, ViewMode viewMode) {
    switch (viewMode) {

        case ViewMode::WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case ViewMode::SOLID_SMOOTH:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader.useShaderProgram();
            shader.setBool(UNIFORM_POLYGONAL_VIEW, false);
            break;
        case ViewMode::SOLID_POLYGONAL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader.useShaderProgram();
            shader.setBool(UNIFORM_POLYGONAL_VIEW, true);
            break;
    }

}

void nextViewMode(gps::Shader shader, ViewMode &viewMode) {
    switch (viewMode) {

        case ViewMode::WIREFRAME:
            viewMode = ViewMode::SOLID_SMOOTH;
            break;
        case ViewMode::SOLID_SMOOTH:
            viewMode = ViewMode::SOLID_POLYGONAL;
            break;
        case ViewMode::SOLID_POLYGONAL:
            viewMode = ViewMode::WIREFRAME;
            break;
    }

    setViewMode(shader, viewMode);
}
