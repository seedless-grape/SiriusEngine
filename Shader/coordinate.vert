/* 坐标轴顶点着色器GLGS */

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 modelMatrix;   // 模型矩阵
uniform mat4 spaceMatrix;   // 投影矩阵 * 视角矩阵

void main()
{
    color = aColor;
    gl_Position = spaceMatrix * modelMatrix * vec4(aPos, 1.0f);
}
