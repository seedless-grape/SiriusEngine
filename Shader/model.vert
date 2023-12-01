/* ģ�Ͷ�����ɫ��GLSL */

#version 330 core

// buffer����
layout (location = 0) in vec3 aPos;         // ��������
layout (location = 1) in vec3 aNormal;      // ��������
layout (location = 2) in vec2 aTexCoords;   // ��������

// ���ֵ
out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} vs_out;


// ȫ�ֱ���
uniform mat4 modelMatrix;       // ����ռ����model
uniform mat4 spaceMatrix;       // ��Ļ�ռ����projection * view
uniform mat4 lightSpaceMatrix;  // ���տռ����

void main() {
    vs_out.fragPos = vec3(modelMatrix * vec4(aPos, 1.0f));
    vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    vs_out.texCoords = aTexCoords;

    gl_Position = spaceMatrix * vec4(vs_out.fragPos, 1.0f);
}