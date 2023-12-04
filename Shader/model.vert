/* ģ�Ͷ�����ɫ��GLSL */

#version 330 core

// buffer����
layout (location = 0) in vec3 aPos;         // ��������
layout (location = 1) in vec3 aNormal;      // ��������
layout (location = 2) in vec2 aTexCoords;   // ��������
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

// ���ֵ
out VS_OUT {
    vec3 fragPos;
    vec2 texCoords;
    vec4 fragPosLightSpace;
    mat3 TBN;
} vs_out;


// ȫ�ֱ���
uniform mat4 modelMatrix;       // ����ռ����model
uniform mat4 spaceMatrix;       // ��Ļ�ռ����projection * view
uniform mat4 lightSpaceMatrix;  // ���տռ����

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