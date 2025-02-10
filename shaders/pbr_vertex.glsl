#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoords = aTexCoords;
    
    // Transformamos la normal y el tangente al espacio mundo
    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);
    // Re-ortogonalizamos el tangente respecto a la normal
    T = normalize(T - N * dot(N, T));
    vec3 B = normalize(cross(N, T));
    
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * worldPos;
}
