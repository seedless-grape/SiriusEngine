/* ģ��Ƭ����ɫ��GLSL */

#version 330 core

#define MAX_POINT_LIGHTS 10
#define GAMMA 2.2

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

    float constant;
    float linear;
    float quadratic;

    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// ����
in VS_OUT {
    vec3 fragPos;
    vec2 texCoords;  
    vec4 fragPosLightSpace; 
    mat3 TBN; 
} fs_in;

// ���
out vec4 fragColor; // Ƭ����ɫ

// ȫ�ֱ���
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightsNum;

uniform vec3 objectColor;
uniform int postProcessing;

uniform sampler2D textureDiffuse1;  // �����������ͼ1
uniform sampler2D textureSpecular1; // ���淴�������ͼ1
uniform sampler2D textureNormal1;   // ������ͼ1
uniform sampler2D textureRough1;    // �ֲڶ���ͼ1

uniform sampler2D shadowMap;    // ��Ӱ��ͼ
uniform bool shadowOn;          // �Ƿ�����Ӱ
uniform bool softShadow;        // PCF����Ӱ
uniform float biasValue;        // ��Ӱƫ����

uniform bool gamma;

const float PI = 3.14159265359;

// ����ǰ������
float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic);
// vec3 CalcDirLightKernel(DirLight light, vec3 normal, vec3 viewDir, float shininess, float kernel[9]);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic);
float CalcShadow(vec4 fragPosLightSpace);

void main() {
    // ��������ɫ
    vec3 albedo;
    if (gamma)
        albedo = pow(texture(textureDiffuse1, fs_in.texCoords).rgb, vec3(2.2));
    else
        albedo = texture(textureDiffuse1, fs_in.texCoords).rgb;

    // ������
    float metallic = texture(textureSpecular1, fs_in.texCoords).r;

    // �ֲڶ�
    float roughness = texture(textureRough1, fs_in.texCoords).r;

    // ����
    vec3 norm = texture(textureNormal1, fs_in.texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);   
    norm = normalize(fs_in.TBN * norm);

    // ����
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    // ���䷴���ʣ�����ʣ������ϣ�ʹ��0.04������ʹ�÷�������ɫ
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // �����
    vec3 Lo = vec3(0.2f) * albedo;     // ��ʼһ����������

    // ���Դ����
    for (int i = 0; i < pointLightsNum; i++) {
        Lo += CalcPointLight(pointLights[i], norm, fs_in.fragPos, viewDir, F0, albedo, roughness, metallic);
    }

    // �����Դ����
    Lo += CalcDirLight(dirLight, norm, fs_in.fragPos, viewDir, F0, albedo, roughness, metallic);

    // gamma
    if (gamma)
        Lo = pow(Lo, vec3(1.0f / GAMMA));

    // ����
    if (postProcessing == 1)
        fragColor = vec4(vec3(1.0f - Lo), 1.0f);
    else if (postProcessing == 2) {
        fragColor = vec4(Lo, 1.0f);
        float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
        fragColor = vec4(average, average, average, 1.0);
    }
    else
        fragColor = vec4(Lo, 1.0f);

}

// GGX���߷ֲ�����
float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfwayDir), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

// Schlick GGX���������
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

// Smith�������ڵ�����
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// fresnel��Schlick����
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

// ��������
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 lightDir = normalize(-light.direction);        // �����
    vec3 halfwayDir = normalize(lightDir + viewDir);    // ������
    vec3 radiance = light.diffuse;                      // radiance

    // ���߷ֲ�����
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    // �������ڵ�
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    // fresnel��
    vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);

    // BRDF���������
    vec3 numerator = NDF * G * F;   
    // BRDF�������ĸ
    float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001;
    // BRDF������
    vec3 specular = numerator / denominator;
    
    vec3 Ks = F;                // ���淴����
    vec3 Kd = vec3(1.0f) - Ks;  // ��������
    Kd *= 1.0f - metallic;      // ������

    float NdotL = max(dot(normal, lightDir), 0.0f);

    float shadow = shadowOn ? CalcShadow(fs_in.fragPosLightSpace) : 0.0f;

    // BRDF
    return (Kd * albedo / PI + specular) * radiance * NdotL * (1.0f - shadow);
}

// ������Դ��radiance
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic) {
    // �����
    vec3 lightDir = normalize(light.position - fragPos);

    // ������
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // ��Դ����
    float distance = length(light.position - fragPos);

    // attenuation
    float attenuation = 1.0 / (distance * distance);

    // radiance
    vec3 radiance = light.diffuse * attenuation;

    // Cook-Torrance BRDF

    // ���߷ֲ�����
    float NDF = DistributionGGX(normal, halfwayDir, roughness);

    // �������ڵ�
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);

    // fresnel��
    vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);

    // BRDF���������
    vec3 numerator = NDF * G * F;

    // BRDF�������ĸ
    float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001;

    // BRDF������
    vec3 specular = numerator / denominator;

    // ���淴���ʣ���ͬ��fresnel��
    vec3 Ks = F;

    // �����غ㣺�����;��淴��ⲻ�ܸ���1.0�����Ǳ��淢�⣩
    // ������������KdӦ����1.0-Ks
    vec3 Kd = vec3(1.0f) - Ks;

    // Kd����1-������ʹ��ֻ�зǽ�������������
    // ��������ǽ�������Ϊ���Ի�ϣ�������û������⣩
    Kd *= 1.0f - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0f);

    // BRDF
    return (Kd * albedo / PI + specular) * radiance * NdotL;
}

float CalcShadow(vec4 fragPosLightSpace) {
    // ͸�ӷָ�(���ü��ռ�����[-w,w]�任��[-1,1])
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �����ֵ��Χת����[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // ��ȡ��ǰƬԪ�������Ϣ������zֵ
    float currentDepth = projCoords.z;
    // ��Ӱƫ��
    float bias = biasValue;

    float shadow;
    // PCF����Ӱ
    if (softShadow) {
        shadow = 0.0f;
        vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
        // ��Ӱ��ͼ3x3��Χ��ȡ�����Ϣ
        for(int x = -1; x <= 1; x++) {
            for(int y = -1; y <= 1; y++) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        shadow /= 9.0f;
    } else {
        // ����Ӱͼ�вɼ���ǰλ�õ������Ϣ(����ϢΪ��ǰ�����µ�������)
        float closestDepth = texture(shadowMap, projCoords.xy).r; 
        // ��ǰƬԪ�������Ϣ������Ӱͼ�е������Ϣ(����һ��ƫ����)ʱ��������Ӱ
        shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    }

    // ���������Զƽ�����ɫ
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}