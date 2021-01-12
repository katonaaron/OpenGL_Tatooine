#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 posLocal;
    vec3 posEye;
    vec4 posLightSpace;
    vec3 normal;
    vec2 texCoords;
} gs_in[];

out vec3 fPosEye;
out vec4 fPosLightSpace;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat3 normalMatrix;
uniform bool polygonalView;

vec3 GetNormal()
{
    vec3 a = vec3(gs_in[1].posLocal) - vec3(gs_in[0].posLocal);
    vec3 b = vec3(gs_in[2].posLocal) - vec3(gs_in[1].posLocal);
    return cross(a, b);
}

void main() {
    vec3 triangleNormal = normalize(normalMatrix * GetNormal());

    // Process the vertices of the triangle
    // Pass-through the data from the vertex shader
    // except for the normal which is selected between
    // the vertex and triangle normal, based on the polygonalView flag
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        fPosEye = gs_in[i].posEye;
        fPosLightSpace = gs_in[i].posLightSpace;
        if (polygonalView) {
            fNormal = triangleNormal;
        } else {
            fNormal = gs_in[i].normal;
        }
        fTexCoords = gs_in[i].texCoords;
        EmitVertex();
    }
    EndPrimitive();
}
