// debug_uv_fragment.glsl
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

void main(){
    FragColor = vec4(TexCoords, 0.0, 1.0);
}