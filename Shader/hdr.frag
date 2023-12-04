#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main() {             
    vec3 hdrColor = texture(hdrBuffer, texCoords).rgb;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    fragColor = vec4(result, 1.0);

}