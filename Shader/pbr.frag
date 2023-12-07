/* 模型片段着色器GLSL */

#version 330 core

#define MAX_POINT_LIGHTS 10
#define GAMMA 2.2

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

    float constant;
    float linear;
    float quadratic;

    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 输入
in VS_OUT {
    vec3 fragPos;
    vec2 texCoords;  
    vec4 fragPosLightSpace; 
    mat3 TBN; 
} fs_in;

// 输出
out vec4 fragColor; // 片段着色

// 全局变量
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightsNum;

uniform vec3 objectColor;
uniform int postProcessing;

uniform sampler2D textureDiffuse1;  // 漫反射材质贴图1
uniform sampler2D textureSpecular1; // 镜面反射材质贴图1
uniform sampler2D textureNormal1;   // 法线贴图1
uniform sampler2D textureRough1;    // 粗糙度贴图1

uniform sampler2D shadowMap;    // 阴影贴图
uniform bool shadowOn;          // 是否开启阴影
uniform bool softShadow;        // PCF软阴影
uniform float biasValue;        // 阴影偏移量

uniform bool gamma;

const float PI = 3.14159265359;

// 函数前向声明
float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic);
// vec3 CalcDirLightKernel(DirLight light, vec3 normal, vec3 viewDir, float shininess, float kernel[9]);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic);
float CalcShadow(vec4 fragPosLightSpace);

void main() {
    // 反射率颜色
    vec3 albedo;
    if (gamma)
        albedo = pow(texture(textureDiffuse1, fs_in.texCoords).rgb, vec3(2.2));
    else
        albedo = texture(textureDiffuse1, fs_in.texCoords).rgb;

    // 金属度
    float metallic = texture(textureSpecular1, fs_in.texCoords).r;

    // 粗糙度
    float roughness = texture(textureRough1, fs_in.texCoords).r;

    // 法线
    vec3 norm = texture(textureNormal1, fs_in.texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);   
    norm = normalize(fs_in.TBN * norm);

    // 视线
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    // 入射反射率：电介质（如塑料）使用0.04；金属使用反射率颜色
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // 反射光
    vec3 Lo = vec3(0.2f) * albedo;     // 初始一个环境光照

    // 点光源计算
    for (int i = 0; i < pointLightsNum; i++) {
        Lo += CalcPointLight(pointLights[i], norm, fs_in.fragPos, viewDir, F0, albedo, roughness, metallic);
    }

    // 定向光源计算
    Lo += CalcDirLight(dirLight, norm, fs_in.fragPos, viewDir, F0, albedo, roughness, metallic);

    // gamma
    if (gamma)
        Lo = pow(Lo, vec3(1.0f / GAMMA));

    // 后处理
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

// GGX法线分布函数
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

// Schlick GGX几何项近似
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

// Smith几何自遮挡函数
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// fresnel项Schlick近似
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

// 定向光计算
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 lightDir = normalize(-light.direction);        // 入射角
    vec3 halfwayDir = normalize(lightDir + viewDir);    // 反射半角
    vec3 radiance = light.diffuse;                      // radiance

    // 法线分布函数
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    // 几何自遮挡
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    // fresnel项
    vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);

    // BRDF镜面项分子
    vec3 numerator = NDF * G * F;   
    // BRDF镜面项分母
    float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001;
    // BRDF镜面项
    vec3 specular = numerator / denominator;
    
    vec3 Ks = F;                // 镜面反射率
    vec3 Kd = vec3(1.0f) - Ks;  // 漫反射率
    Kd *= 1.0f - metallic;      // 金属率

    float NdotL = max(dot(normal, lightDir), 0.0f);

    float shadow = shadowOn ? CalcShadow(fs_in.fragPosLightSpace) : 0.0f;

    // BRDF
    return (Kd * albedo / PI + specular) * radiance * NdotL * (1.0f - shadow);
}

// 计算点光源的radiance
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float roughness, float metallic) {
    // 入射角
    vec3 lightDir = normalize(light.position - fragPos);

    // 反射半角
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 光源距离
    float distance = length(light.position - fragPos);

    // attenuation
    float attenuation = 1.0 / (distance * distance);

    // radiance
    vec3 radiance = light.diffuse * attenuation;

    // Cook-Torrance BRDF

    // 法线分布函数
    float NDF = DistributionGGX(normal, halfwayDir, roughness);

    // 几何自遮挡
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);

    // fresnel项
    vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);

    // BRDF镜面项分子
    vec3 numerator = NDF * G * F;

    // BRDF镜面项分母
    float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001;

    // BRDF镜面项
    vec3 specular = numerator / denominator;

    // 镜面反射率（等同于fresnel）
    vec3 Ks = F;

    // 能量守恒：漫射光和镜面反射光不能高于1.0（除非表面发光）
    // 因此漫反射分量Kd应等于1.0-Ks
    vec3 Kd = vec3(1.0f) - Ks;

    // Kd乘以1-金属度使得只有非金属具有漫射照
    // 如果部分是金属，则为线性混合（纯金属没有漫射光）
    Kd *= 1.0f - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0f);

    // BRDF
    return (Kd * albedo / PI + specular) * radiance * NdotL;
}

float CalcShadow(vec4 fragPosLightSpace) {
    // 透视分割(将裁剪空间坐标[-w,w]变换成[-1,1])
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 将深度值范围转换至[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // 获取当前片元的深度信息，就是z值
    float currentDepth = projCoords.z;
    // 阴影偏移
    float bias = biasValue;

    float shadow;
    // PCF软阴影
    if (softShadow) {
        shadow = 0.0f;
        vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
        // 阴影贴图3x3范围内取深度信息
        for(int x = -1; x <= 1; x++) {
            for(int y = -1; y <= 1; y++) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        shadow /= 9.0f;
    } else {
        // 从阴影图中采集当前位置的深度信息(该信息为当前坐标下的最近深度)
        float closestDepth = texture(shadowMap, projCoords.xy).r; 
        // 当前片元的深度信息大于阴影图中的深度信息(加上一个偏移量)时，产生阴影
        shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    }

    // 解决超出光远平面的着色
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}