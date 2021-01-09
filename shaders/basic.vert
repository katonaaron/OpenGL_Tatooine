#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out VS_OUT {
    vec3 posLocal;
    vec3 posEye;
    vec4 posLightSpace;
    vec3 normal;
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;

void main()
{
    vec4 posLocal = vec4(vPosition, 1.0f);
    vec4 posWorld = model * posLocal;
    vec4 posEye = view * posWorld;
    vec4 posProj = projection * posEye;
    vec4 posLightSpace = lightSpaceTrMatrix * posWorld;

    gl_Position = posProj;
    vs_out.posLocal = posLocal.xyz;
    vs_out.posEye = posEye.xyz;
    vs_out.posLightSpace = posLightSpace;
    vs_out.normal = normalMatrix * vNormal;
    vs_out.texCoords = vTexCoords;
}
