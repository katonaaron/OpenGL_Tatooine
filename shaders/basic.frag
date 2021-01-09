#version 410 core

in vec3 fPosEye;
in vec4 fPosLightSpace;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 view;

//lighting
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//constants
float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

void computeDirLight(DirLight light, vec3 normal, vec3 viewDir, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
    // normalize light direction
    vec3 lightDir = vec3(normalize(view * vec4(light.direction, 0.0f)));

    // compute half vector
    vec3 halfVector = normalize(lightDir + viewDir);

    //compute ambient light
    ambient = light.ambient * ambientStrength;

    //compute diffuse light
    float diffCoeff = max(dot(normal, lightDir), 0.0f);
    diffuse = light.diffuse * diffCoeff;

    //compute specular light
    float specCoeff = pow(max(dot(normal, halfVector), 0.0f), shininess);
    specular = light.specular * specCoeff * specularStrength;
}

float computeShadow(DirLight light, vec3 normal)
{
    // normalize light direction
    vec3 lightDir = vec3(normalize(view * vec4(light.direction, 0.0f)));

    // perform perspective divide
    vec3 normalizedCoords = fPosLightSpace.xyz / fPosLightSpace.w;

    // Transform to [0, 1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;

    // if the fragment is outside of the far plane from the light's point of view
    // do not cast shadow on it
    if (normalizedCoords.z > 1.0f)
    return 0.0f;

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

    // Get depth of the current fragment from light's perspective
    float currentDepth = normalizedCoords.z;

    // Compute bias for correcting the shadow acne
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

    // Check whether the current fragment is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}

void main()
{
    // compute normal vector in eye coordinates
    vec3 normal = normalize(fNormal);

    // compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye);

    // color components
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    computeDirLight(dirLight, normal, viewDir, ambient, diffuse, specular);

    //modulate with shadow
    float shadow = computeShadow(dirLight, normal);

    //compute final vertex color
    vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

    fColor = vec4(color, 1.0f);
}
