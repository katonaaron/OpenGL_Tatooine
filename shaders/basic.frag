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

// fog
struct Fog {
    vec3 position;
    vec4 color;
    float density;
};
uniform Fog fog;

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

    // transform to [0, 1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;

    // if the fragment is outside of the far plane from the light's point of view
    // do not cast shadow on it
    if (normalizedCoords.z > 1.0f)
    return 0.0f;

    // get depth of the current fragment from light's perspective
    float currentDepth = normalizedCoords.z;

    // compute bias for correcting the shadow acne
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

    /// Apply percentage-close filtering for increasing the shadow quality

    // variable for storing the final shadow value
    float shadow = 0.0;

    // size of a texture pixel (width, height)
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // check current and nearby depths in the shadowMap
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            // the closest depth for the given texture coordinates
            float closestDepth = texture(shadowMap, normalizedCoords.xy + vec2(x, y) * texelSize).r;

            // verify if the fragement is in shadow in the given texture coordinate
            shadow += currentDepth - bias > closestDepth ? 1.0f : 0.0f;
        }
    }

    // compute the average of the shadow values
    shadow /= 9.0;

    return shadow;
}

float computeFogFactor(Fog fog) {
    float fragmentDistance = length(fog.position - fPosEye);
    float fogFactor =  exp(-pow(fragmentDistance * fog.density, 2));
    return clamp(fogFactor, 0.0f, 1.0f);
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

    // modulate with shadow
    float shadow = computeShadow(dirLight, normal);
    ambient *= 1.0f;// no shadow
    diffuse *= 1.0f - shadow;
    specular *= 1.0f - shadow;

    // apply texture
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    // compute color
    vec4 color = vec4(ambient + diffuse + specular, 1.0f);

    // apply fog effect
    float fogFactor = computeFogFactor(fog);
    color = mix(fog.color, color, fogFactor);

    // staturate color at white
    color = min(color, 1.0f);

    fColor = color;
}
