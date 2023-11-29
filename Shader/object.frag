/* ����Ƭ����ɫ��GLGS */

#version 330 core

#define MAX_POINT_LIGHTS 10
#define GAMMA 2.2

out vec4 fragColor;

// ����
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// �����
struct DirLight {
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// ���Դ
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

    if (postProcessing <= 2) {  // �Ǻ˴���
        // �����
        if (dirLight.enabled)
            result += CalcDirLight(dirLight, norm, viewDir);

        // ���Դ
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
        }
        
        // ����
        if (postProcessing == 1)
            fragColor = vec4(vec3(1.0f - objectColor * result), 1.0f);
        // �Ҷ�
        else if (postProcessing == 2) {
            fragColor = vec4(objectColor * result, 1.0f);
            float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
            fragColor = vec4(average, average, average, 1.0);
        }
        // ��Ч��
        else
            fragColor = vec4(objectColor * result, 1.0f);
    } else {     // �˴���
        float kernel[9];
        // ��
        if (postProcessing == 3)
            kernel = float[](
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );
        // ģ��
        else if (postProcessing == 4)
            kernel = float[](
                1.0 / 16, 2.0 / 16, 1.0 / 16,
                2.0 / 16, 4.0 / 16, 2.0 / 16,
                1.0 / 16, 2.0 / 16, 1.0 / 16  
            );
        // ��Ե
        else if (postProcessing == 5)
            kernel = float[](
                1,  1, 1,
                1, -8, 1,
                1,  1, 1  
            );
        
        // �����
        if (dirLight.enabled)
            result += CalcDirLightKernel(dirLight, norm, viewDir, kernel);

        // ���Դ
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLightKernel(pointLights[i], norm, fragPos, viewDir, kernel);
        }

        fragColor = vec4(objectColor * result, 1.0f);
        if (gamma)
            fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
    }

}

// ��������
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

// �����Ӻ˼���
vec3 CalcDirLightKernel(DirLight light, vec3 norm, vec3 viewDir, float kernel[9]) {
    // ����ƫ��
    const float offset = 1.0 / 300.0;
    
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
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

// ���Դ����
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

// ���Դ�Ӻ˼���
vec3 CalcPointLightKernel(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, float kernel[9]) {
    // ����ƫ��
    const float offset = 1.0 / 300.0;
    
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
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
