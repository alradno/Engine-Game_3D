#version 330 core

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D albedoMap;           // sRGB
uniform sampler2D metallicRoughnessMap;  // Red: metallic, Green: roughness
uniform sampler2D normalMap;             // Normal map
uniform bool useMaps;
uniform vec3 camPos;
uniform vec3 ambientColor;

const float PI = 3.14159265359;

struct Light {
    vec4 typeAndPadding;    // x: type (int), yzw: padding
    vec4 position;          // xyz: position, w: padding
    vec4 direction;         // xyz: direction, w: padding
    vec4 colorAndIntensity; // rgb: color, a: intensity
    vec4 spotParams;        // x: cutOff, y: outerCutOff, z,w: padding
};

layout(std140) uniform LightBlock {
    Light lights[10];
};

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = PI * pow(NdotH * NdotH * (a2 - 1.0) + 1.0, 2.0);
    return a2 / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float k = pow(roughness + 1.0, 2.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float ggx1 = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx2 = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}

void main() {
    vec4 albedoSample = texture(albedoMap, TexCoords);
    vec3 albedoColor = albedoSample.rgb;
    float alpha = albedoSample.a;
    
    float metallic = useMaps ? texture(metallicRoughnessMap, TexCoords).r : 0.0;
    float roughness = useMaps ? texture(metallicRoughnessMap, TexCoords).g : 1.0;
    vec3 tangentNormal = useMaps ? texture(normalMap, TexCoords).rgb : vec3(0.5, 0.5, 1.0);
    tangentNormal = tangentNormal * 2.0 - 1.0;
    // Invertir el canal verde si es necesario.
    tangentNormal.y = -tangentNormal.y;
    vec3 N = normalize(TBN * tangentNormal);
    
    vec3 F0 = mix(vec3(0.04), albedoColor, metallic);
    vec3 V = normalize(camPos - FragPos);
    
    vec3 result = vec3(0.0);
    for (int i = 0; i < 10; ++i) {
        if (int(lights[i].typeAndPadding.x) == -1)
            continue;
        
        vec3 lightPos = lights[i].position.xyz;
        vec3 L = normalize(lightPos - FragPos);
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * NdotL + 0.001);
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        
        int lightType = int(lights[i].typeAndPadding.x);
        if (lightType == 0) {
            result += (kD * albedoColor / PI + specular) * lights[i].colorAndIntensity.rgb * lights[i].colorAndIntensity.a * NdotL;
        } else if (lightType == 1) {
            float cutOff = lights[i].spotParams.x;
            float outerCutOff = lights[i].spotParams.y;
            float theta = dot(L, normalize(-lights[i].direction.xyz));
            float epsilon = cutOff - outerCutOff;
            float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
            result += intensity * ((kD * albedoColor / PI + specular) *
                      lights[i].colorAndIntensity.rgb * lights[i].colorAndIntensity.a * NdotL);
        }
    }
    
    result += ambientColor * albedoColor;
    FragColor = vec4(result, alpha);
}