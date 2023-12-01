/* 模型顶点着色器GLSL */

#version 330 core

// buffer传入
layout (location = 0) in vec3 aPos;         // 顶点坐标
layout (location = 1) in vec3 aNormal;      // 法线坐标
layout (location = 2) in vec2 aTexCoords;   // 纹理坐标

// 输出值
out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} vs_out;


// 全局变量
uniform mat4 modelMatrix;       // 世界空间矩阵model
uniform mat4 spaceMatrix;       // 屏幕空间矩阵projection * view
uniform mat4 lightSpaceMatrix;  // 光照空间矩阵

void main() {
    vs_out.fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));
    vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    vs_out.texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(vs_out.fragPos, 1.0f);
}