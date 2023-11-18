/* 物体顶点着色器GLGS */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 spaceMatrix;  // projection * view

void main()
{
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));  // reduce from 4 to 3
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;  // normal matrix
    texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(fragPos, 1.0f);  // save computing capacity
}
