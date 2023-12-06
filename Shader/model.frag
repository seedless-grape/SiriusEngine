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
// uniform float shininess;

uniform sampler2D textureDiffuse1;  // 漫反射材质贴图1
uniform sampler2D textureSpecular1; // 镜面反射材质贴图1
uniform sampler2D textureNormal1;
uniform sampler2D textureRough1;

uniform sampler2D shadowMap;    // 阴影贴图
uniform bool shadowOn;          // 是否开启阴影
uniform bool softShadow;        // PCF软阴影
uniform float biasValue;        // 阴影偏移量

uniform bool gamma;

// 函数前向声明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shininess);
vec3 CalcDirLightKernel(DirLight light, vec3 normal, vec3 viewDir, float shininess, float kernel[9]);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess);
vec3 CalcPointLightKernel(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess, float kernel[9]);
float CalcShadow(vec4 fragPosLightSpace);

void main() {
    vec3 norm = texture(textureNormal1, fs_in.texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);   
    norm = normalize(fs_in.TBN * norm);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    float shininess = texture(textureRough1, fs_in.texCoords).r * 32.0f;
    
    vec3 result = vec3(0.0f);
    
    if (postProcessing <= 2) {  // 非核处理
        // 定向光
        if (dirLight.enabled)
            result += CalcDirLight(dirLight, norm, viewDir, shininess);

        // 点光源
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLight(pointLights[i], norm, fs_in.fragPos, viewDir, shininess);
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
            result += CalcDirLightKernel(dirLight, norm, viewDir, shininess, kernel);

        // 点光源
        for (int i = 0; i < pointLightsNum; i++) {
            if (pointLights[i].enabled)
                result += CalcPointLightKernel(pointLights[i], norm, fs_in.fragPos, viewDir, shininess, kernel);
        }

        fragColor = vec4(objectColor * result, 1.0f);
    }

    if (gamma)
        fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
        
}

// 定向光计算
vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, float shininess) {
    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), shininess);

    // shadow
    float shadow = shadowOn ? CalcShadow(fs_in.fragPosLightSpace) : 0.0f;

    // result
    vec3 ambient = light.ambient * vec3(texture(textureDiffuse1, fs_in.texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuse1, fs_in.texCoords));
    vec3 specular = light.specular * spec * vec3(texture(textureSpecular1, fs_in.texCoords));

    return ambient + (1.0f - shadow) * (diffuse + specular);
}

// 定向光加核计算
vec3 CalcDirLightKernel(DirLight light, vec3 norm, vec3 viewDir, float shininess, float kernel[9]) {
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
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), shininess);
    
    // result
    for(int i = 0; i < 9; i++) {
        vec3 ambient = light.ambient * vec3(texture(textureDiffuse1, fs_in.texCoords + offsets[i]));
        vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuse1, fs_in.texCoords + offsets[i]));
        vec3 specular = light.specular * spec * vec3(texture(textureSpecular1, fs_in.texCoords + offsets[i]));
        sampleTex[i] = ambient + diffuse + specular;
    }

    vec3 tempRes = vec3(0.0f);
    for (int i = 0; i < 9; i++)
        tempRes += sampleTex[i] * kernel[i];

    return tempRes;
}

// 点光源计算
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, float shininess) {
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), shininess);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // result
    vec3 ambient = light.ambient * vec3(texture(textureDiffuse1, fs_in.texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuse1, fs_in.texCoords));
    vec3 specular = light.specular * spec * vec3(texture(textureSpecular1, fs_in.texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// 点光源加核计算
vec3 CalcPointLightKernel(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, float shininess, float kernel[9]) {
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
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), shininess);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // result
    for(int i = 0; i < 9; i++) {
        vec3 ambient = light.ambient * vec3(texture(textureDiffuse1, fs_in.texCoords + offsets[i]));
        vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuse1, fs_in.texCoords + offsets[i]));
        vec3 specular = light.specular * spec * vec3(texture(textureSpecular1, fs_in.texCoords + offsets[i]));
        sampleTex[i] = (ambient + diffuse + specular) * attenuation;
    }

    vec3 tempRes = vec3(0.0f);
    for (int i = 0; i < 9; i++)
        tempRes += sampleTex[i] * kernel[i];

    return tempRes;
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