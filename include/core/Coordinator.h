#pragma once

#include "EntityManager.h"
#include "core/ComponentManager.h"
#include "systems/SystemManager.h"
#include <memory>
#include <vector>

class Coordinator {
public:
    void Init() {
        mEntityManager = std::make_unique<EntityManager>();
        mComponentManager = std::make_unique<ComponentManager>();
        mSystemManager = std::make_unique<SystemManager>();
    }

    // Métodos para entidades
    ECS::Entity CreateEntity() {
        return mEntityManager->CreateEntity();
    }

    void DestroyEntity(ECS::Entity entity) {
        mEntityManager->DestroyEntity(entity);
        mComponentManager->EntityDestroyed(entity);
        mSystemManager->EntityDestroyed(entity);
    }

    // Métodos para componentes
    template <typename T>
    void RegisterComponent() {
        mComponentManager->RegisterComponent<T>();
    }

    template <typename T>
    void AddComponent(ECS::Entity entity, T component) {
        mComponentManager->AddComponent<T>(entity, component);
        auto signature = mEntityManager->GetSignature(entity);
        signature.set(ECS::GetComponentTypeID<T>(), true);
        mEntityManager->SetSignature(entity, signature);
        mSystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    void RemoveComponent(ECS::Entity entity) {
        mComponentManager->RemoveComponent<T>(entity);
        auto signature = mEntityManager->GetSignature(entity);
        signature.set(ECS::GetComponentTypeID<T>(), false);
        mEntityManager->SetSignature(entity, signature);
        mSystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    T &GetComponent(ECS::Entity entity) {
        return mComponentManager->GetComponent<T>(entity);
    }

    template <typename T>
    ECS::ComponentType GetComponentType() {
        return ECS::GetComponentTypeID<T>();
    }

    // Métodos para sistemas
    template <typename T>
    std::shared_ptr<T> RegisterSystem() {
        return mSystemManager->RegisterSystem<T>();
    }

    template <typename T>
    void SetSystemSignature(ECS::Signature signature) {
        mSystemManager->SetSignature<T>(signature);
    }

    template <typename T>
    std::vector<ECS::Entity> GetEntitiesWithComponent() {
        std::vector<ECS::Entity> entities;
        ECS::ComponentType compType = GetComponentType<T>();
        // Se asume que ECS::MAX_ENTITIES define el rango de entidades posibles.
        for (ECS::Entity entity = 0; entity < ECS::MAX_ENTITIES; ++entity) {
            auto signature = mEntityManager->GetSignature(entity);
            if (signature.test(compType)) {
                entities.push_back(entity);
            }
        }
        return entities;
    }
    
    // Nuevo método para limpiar todas las entidades (reset del ECS)
    void Clear() {
        // Se notifica la destrucción a todos los sistemas y componentes de cada entidad.
        for (ECS::Entity entity = 0; entity < ECS::MAX_ENTITIES; ++entity) {
            mComponentManager->EntityDestroyed(entity);
            mSystemManager->EntityDestroyed(entity);
        }
        // Se "reinicia" el EntityManager
        mEntityManager = std::make_unique<EntityManager>();
    }

private:
    std::unique_ptr<EntityManager> mEntityManager;
    std::unique_ptr<ComponentManager> mComponentManager;
    std::unique_ptr<SystemManager> mSystemManager;
};