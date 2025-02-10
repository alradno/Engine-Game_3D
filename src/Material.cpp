// #include "Material.h"
// #include "glm/gtc/type_ptr.hpp"

// Material::Material()
//     : albedoColor(1.0f), metalness(0.0f), roughness(1.0f), ao(1.0f),
//       isTransparent(false)
// {
//     Logger::Debug("[Material] Default material created.");
// }

// void Material::Apply(const Shader &shader) const {
//     int loc = glGetUniformLocation(shader.ID, "material_isTransparent");
//     if (loc == -1) {
//         Logger::Warning("[Material::Apply] Uniform 'material_isTransparent' not found.");
//     } else {
//         glUniform1i(loc, isTransparent ? 1 : 0);
//     }

//     bool useAlbedoMap = (albedoMap != nullptr);
//     bool useNormalMap = (normalMap != nullptr);
//     bool useMRMap = (metallicRoughnessMap != nullptr);
//     bool useAOMap = (aoMap != nullptr);

//     glUniform1i(glGetUniformLocation(shader.ID, "material_useAlbedoMap"), useAlbedoMap ? 1 : 0);
//     glUniform1i(glGetUniformLocation(shader.ID, "material_useNormalMap"), useNormalMap ? 1 : 0);
//     glUniform1i(glGetUniformLocation(shader.ID, "material_useMetallicRoughnessMap"), useMRMap ? 1 : 0);
//     glUniform1i(glGetUniformLocation(shader.ID, "material_useAOMap"), useAOMap ? 1 : 0);

//     int textureUnit = 0;
//     if (useAlbedoMap) {
//         glActiveTexture(GL_TEXTURE0 + textureUnit);
//         glBindTexture(GL_TEXTURE_2D, albedoMap->ID);
//         glUniform1i(glGetUniformLocation(shader.ID, "albedoMap"), textureUnit);
//         textureUnit++;
//     }
//     if (useNormalMap) {
//         glActiveTexture(GL_TEXTURE0 + textureUnit);
//         glBindTexture(GL_TEXTURE_2D, normalMap->ID);
//         glUniform1i(glGetUniformLocation(shader.ID, "normalMap"), textureUnit);
//         textureUnit++;
//     }
//     if (useMRMap) {
//         glActiveTexture(GL_TEXTURE0 + textureUnit);
//         glBindTexture(GL_TEXTURE_2D, metallicRoughnessMap->ID);
//         glUniform1i(glGetUniformLocation(shader.ID, "metallicRoughnessMap"), textureUnit);
//         textureUnit++;
//     }
//     if (useAOMap) {
//         glActiveTexture(GL_TEXTURE0 + textureUnit);
//         glBindTexture(GL_TEXTURE_2D, aoMap->ID);
//         glUniform1i(glGetUniformLocation(shader.ID, "aoMap"), textureUnit);
//         textureUnit++;
//     }
    
//     glUniform3fv(glGetUniformLocation(shader.ID, "material_albedoColor"), 1, glm::value_ptr(albedoColor));
//     glUniform1f(glGetUniformLocation(shader.ID, "material_metalness"), metalness);
//     glUniform1f(glGetUniformLocation(shader.ID, "material_roughness"), roughness);
//     glUniform1f(glGetUniformLocation(shader.ID, "material_ao"), ao);
// }
