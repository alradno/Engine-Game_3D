#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

// Definir tipos de luz mediante un entero:
// 0: Luz puntual (point light)
// 1: Luz focal (spot light)
// 2: Luz direccional (directional light) (opcional)
enum class LightType : int {
    POINT = 0,
    SPOT = 1,
    DIRECTIONAL = 2
};

// Estructura para almacenar la información de una luz en formato std140,
// usando vec4 para garantizar una alineación correcta (80 bytes por luz).
struct Light {
    // Almacena el tipo en la componente x; las componentes yzw se usan como padding.
    glm::vec4 typeAndPadding;
    // Posición de la luz: xyz y w se usa como padding.
    glm::vec4 position;
    // Dirección de la luz: xyz y w se usa como padding.
    glm::vec4 direction;
    // Color e intensidad: rgb = color, a = intensidad.
    glm::vec4 colorAndIntensity;
    // Parámetros para luces focales: x = cutOff, y = outerCutOff, z y w como padding.
    glm::vec4 spotParams;
};

#endif // LIGHT_H
