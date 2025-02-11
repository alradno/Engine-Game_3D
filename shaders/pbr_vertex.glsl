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
    
    // Calcular la matriz normal para transformar tanto la normal como el tangente
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    // Re-ortogonalizar el tangente respecto a la normal
    T = normalize(T - N * dot(N, T));
    vec3 B = cross(N, T);
    
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * worldPos;
}
