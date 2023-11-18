/* �����ᶥ����ɫ��GLGS */

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 modelMatrix;   // ģ�;���
uniform mat4 spaceMatrix;   // ͶӰ���� * �ӽǾ���

void main()
{
    color = aColor;
    gl_Position = spaceMatrix * modelMatrix * vec4(aPos, 1.0f);
}
