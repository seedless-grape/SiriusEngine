/* 物体片段着色器GLGS */

#version 330 core

#define MAX_POINT_LIGHTS 10
#define GAMMA 2.2

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
uniform int postProcessing;

uniform bool gamma;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcDirLightKernel(DirLight light, vec3 normal, vec3 viewDir, float kernel[9]);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLightKernel(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float kernel[9]);


void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0f);

    if (postProcessing <= 2) {  // 非核处理
        // 定向光
        if (dirLight.enabled)
            result += CalcDirLight(dirLight, norm, viewDir);

        // 点光源
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
        }
        
        // 反相
        if (postProcessing == 1)
            fragColor = vec4(vec3(1.0f - objectColor * result), 1.0f);
        // 灰度
        else if (postProcessing == 2) {
            fragColor = vec4(objectColor * result, 1.0f);
            float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
            fragColor = vec4(average, average, average, 1.0);
        }
        // 无效果
        else
            fragColor = vec4(objectColor * result, 1.0f);
    } else {     // 核处理
        float kernel[9];
        // 锐化
        if (postProcessing == 3)
            kernel = float[](
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );
        // 模糊
        else if (postProcessing == 4)
            kernel = float[](
                1.0 / 16, 2.0 / 16, 1.0 / 16,
                2.0 / 16, 4.0 / 16, 2.0 / 16,
                1.0 / 16, 2.0 / 16, 1.0 / 16  
            );
        // 边缘
        else if (postProcessing == 5)
            kernel = float[](
                1,  1, 1,
                1, -8, 1,
                1,  1, 1  
            );
        
        // 定向光
        if (dirLight.enabled)
            result += CalcDirLightKernel(dirLight, norm, viewDir, kernel);

        // 点光源
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLightKernel(pointLights[i], norm, fragPos, viewDir, kernel);
        }

        fragColor = vec4(objectColor * result, 1.0f);
        if (gamma)
            fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
    }

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

// 定向光加核计算
vec3 CalcDirLightKernel(DirLight light, vec3 norm, vec3 viewDir, float kernel[9]) {
    // 采样偏移
    const float offset = 1.0 / 300.0;
    
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    vec3 sampleTex[9];

    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess);
    
    // result
    for(int i = 0; i < 9; i++) {
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords + offsets[i]));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords + offsets[i]));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords + offsets[i]));
        sampleTex[i] = ambient + diffuse + specular;
    }

    vec3 tempRes = vec3(0.0f);
    for (int i = 0; i < 9; i++)
        tempRes += sampleTex[i] * kernel[i];

    return tempRes;
}

// 点光源计算
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
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

// 点光源加核计算
vec3 CalcPointLightKernel(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, float kernel[9]) {
    // 采样偏移
    const float offset = 1.0 / 300.0;
    
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    vec3 sampleTex[9];

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
    for(int i = 0; i < 9; i++) {
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords + offsets[i]));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords + offsets[i]));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords + offsets[i]));
        sampleTex[i] = (ambient + diffuse + specular) * attenuation;
    }

    vec3 tempRes = vec3(0.0f);
    for (int i = 0; i < 9; i++)
        tempRes += sampleTex[i] * kernel[i];

    return tempRes;
}
