/* 光源立方体片段着色器GLGS */

#version 330 core

#define GAMMA 2.2

out vec4 fragColor;

uniform vec3 color;
uniform int postProcessing;

uniform bool gamma;

void main()
{
    switch (postProcessing) {
        case 1:
            fragColor = vec4(vec3(1.0f - color), 1.0f);
            break;
        case 2:
            fragColor = vec4(color, 1.0f);
            float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
            fragColor = vec4(average, average, average, 1.0);
            break;
        default:
            fragColor = vec4(color, 1.0f);
    }

    if (gamma)
        fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
    
}
