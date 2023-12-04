/* 模型顶点着色器GLSL */

#version 330 core

// buffer传入
layout (location = 0) in vec3 aPos;         // 顶点坐标
layout (location = 1) in vec3 aNormal;      // 法线坐标
layout (location = 2) in vec2 aTexCoords;   // 纹理坐标
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

// 输出值
out VS_OUT {
    vec3 fragPos;
    vec2 texCoords;
    vec4 fragPosLightSpace;
    mat3 TBN;
} vs_out;


// 全局变量
uniform mat4 modelMatrix;       // 世界空间矩阵model
uniform mat4 spaceMatrix;       // 屏幕空间矩阵projection * view
uniform mat4 lightSpaceMatrix;  // 光照空间矩阵

void main() {
    vec3 T = normalize(vec3(modelMatrix * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(aNormal,    0.0)));
    vs_out.TBN = mat3(T, B, N);

    vs_out.fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));
    vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(vs_out.fragPos, 1.0f);
}