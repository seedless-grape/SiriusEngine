#version 330 core

#define GAMMA 2.2

out vec4 fragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform bool gamma;

void main()
{    
    fragColor = texture(skybox, texCoords);
    if (gamma)
        fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
    
}