#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} gs_in[];

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat3 normalMatrix;
uniform bool polygonalView;

vec3 GetNormal()
{
    vec3 a = vec3(gs_in[1].position) - vec3(gs_in[0].position);
    vec3 b = vec3(gs_in[2].position) - vec3(gs_in[1].position);
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = normalMatrix * GetNormal();

    for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
        gl_Position = gl_in[i].gl_Position;
        fPosition = gs_in[i].position;
        if (polygonalView)
            fNormal = normal;
        else
            fNormal = gs_in[i].normal;
        fTexCoords = gs_in[i].texCoords;
        EmitVertex();
    }
    EndPrimitive();
}
