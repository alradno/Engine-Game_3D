#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D albedoMap;

void main() {
    vec4 texColor = texture(albedoMap, TexCoords);
    // Se asume que la textura tiene el canal alfa correcto para transparencia
    FragColor = texColor;
}
