#version 430 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uiTexture;
uniform vec4 color;

void main()
{
    vec4 sampled = texture(uiTexture, TexCoord);
    FragColor = color * sampled;
}