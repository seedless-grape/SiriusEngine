/* 物体片段着色器GLGS */

#version 330 core

#define MAX_POINT_LIGHTS 10

out vec4 fragColor;

// 材质
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// 定向光
struct DirLight {
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight {
    bool enabled;
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightsNum;

uniform vec3 objectColor;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0f);

    // 定向光
    if (dirLight.enabled)
        result += CalcDirLight(dirLight, norm, viewDir);

    // 点光源
    for (int i = 0; i < pointLightsNum; i++) {
        if (pointLights[i].enabled)
            result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    }
    
    fragColor = vec4(objectColor * result, 1.0f);
}

// 定向光计算
vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess);
    
    // result
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    return ambient + diffuse + specular;
}

// 点光源
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // result
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}
