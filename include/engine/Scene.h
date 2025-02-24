#pragma once

/**
 * @brief Interfaz base para una escena.
 *
 * Define los métodos necesarios para inicializar, actualizar, renderizar y limpiar la escena.
 */
class Scene {
public:
    virtual void Init() = 0;         // Cargar recursos, entidades, sistemas, etc.
    virtual void Update(float dt) = 0; // Lógica de actualización (input, movimiento, etc.)
    virtual void Render() = 0;       // Renderizado de la escena
    virtual void Destroy() = 0;      // Liberar recursos propios de la escena
    virtual ~Scene() {}
};