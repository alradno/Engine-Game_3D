/**
 * @file EntityLoader.h
 * @brief Declaration of the EntityLoader class used to load entities from a YAML configuration file.
 *
 * The loader reads a YAML file and creates entities with their corresponding components
 * (for example, TransformComponent and RenderComponent).
 */

 #ifndef ENTITYLOADER_H
 #define ENTITYLOADER_H
 
 #include "core/Coordinator.h"
 #include <string>
 
 class EntityLoader {
 public:
     /**
      * @brief Loads entities and their components from a YAML file.
      * @param coordinator Pointer to the ECS Coordinator.
      * @param filename Path to the YAML configuration file.
      */
     static void LoadEntitiesFromYAML(Coordinator* coordinator, const std::string& filename);
 };
 
 #endif // ENTITYLOADER_H
 