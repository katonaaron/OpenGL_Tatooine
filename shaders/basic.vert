#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out VS_OUT {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() 
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	vs_out.position = vPosition;
	vs_out.normal = normalMatrix * vNormal;
	vs_out.texCoords = vTexCoords;
}
