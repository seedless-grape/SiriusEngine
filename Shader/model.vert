/* 模型顶点着色器GLSL */

#version 330 core

// buffer传入
layout (location = 0) in vec3 aPos;         // 顶点坐标
layout (location = 1) in vec3 aNormal;      // 法线坐标
layout (location = 2) in vec2 aTexCoords;   // 纹理坐标

// 输出值
out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

// 全局变量
uniform mat4 modelMatrix;         // model
uniform mat4 spaceMatrix;          // view

void main() {
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(fragPos, 1.0f);
}