/* ģ�Ͷ�����ɫ��GLSL */

#version 330 core

// buffer����
layout (location = 0) in vec3 aPos;         // ��������
layout (location = 1) in vec3 aNormal;      // ��������
layout (location = 2) in vec2 aTexCoords;   // ��������

// ���ֵ
out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

// ȫ�ֱ���
uniform mat4 modelMatrix;         // model
uniform mat4 spaceMatrix;          // view

void main() {
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(fragPos, 1.0f);
}