/* 光源立方体顶点着色器GLGS */

#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 spaceMatrix;

void main()
{
    gl_Position = spaceMatrix * modelMatrix * vec4(aPos, 1.0f);
}
