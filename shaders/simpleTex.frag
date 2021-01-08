#version 410 core

in vec2 fTexCoords;

out vec4 fColor;

// textures
uniform sampler2D diffuseTexture;

void main() 
{
    fColor = texture(diffuseTexture, fTexCoords);
}
