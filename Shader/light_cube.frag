/* 光源立方体片段着色器GLGS */

#version 330 core

out vec4 fragColor;

uniform vec3 color;

void main()
{
    fragColor = vec4(color, 1.0f);
}
