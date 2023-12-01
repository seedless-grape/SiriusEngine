/* 物体顶点着色器GLGS */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} vs_out;

uniform mat4 modelMatrix;
uniform mat4 spaceMatrix;  // projection * view

void main()
{
    vs_out.fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));  // reduce from 4 to 3
    vs_out.normal = mat3(transpose(inverse(modelMatrix))) * aNormal;  // normal matrix
    vs_out.texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(vs_out.fragPos, 1.0f);  // save computing capacity
}
