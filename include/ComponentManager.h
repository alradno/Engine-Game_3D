#pragma once

#include "ECS.h"
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <typeinfo>

// Base interface for component arrays
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    // Declare RemoveData in the interface to allow override
    virtual void RemoveData(ECS::Entity entity) = 0;
    virtual void EntityDestroyed(ECS::Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    void InsertData(ECS::Entity entity, T component) {
        mComponentMap[entity] = component;
    }
    // Now this override is valid because RemoveData is declared in IComponentArray.
    void RemoveData(ECS::Entity entity) override {
        mComponentMap.erase(entity);
    }
    T& GetData(ECS::Entity entity) {
        return mComponentMap.at(entity);
    }
    bool HasData(ECS::Entity entity) {
        return mComponentMap.find(entity) != mComponentMap.end();
    }
    void EntityDestroyed(ECS::Entity entity) override {
        mComponentMap.erase(entity);
    }
private:
    std::unordered_map<ECS::Entity, T> mComponentMap;
};

class ComponentManager {
public:
    template <typename T>
    void RegisterComponent() {
        const char* typeName = typeid(T).name();
        if (mComponentArrays.find(typeName) != mComponentArrays.end()) {
            throw std::runtime_error("Registering component type more than once.");
        }
        mComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
    }
    
    template <typename T>
    void AddComponent(ECS::Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }
    
    template <typename T>
    void RemoveComponent(ECS::Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }
    
    template <typename T>
    T& GetComponent(ECS::Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }
    
    void EntityDestroyed(ECS::Entity entity) {
        for (auto const& pair : mComponentArrays) {
            pair.second->EntityDestroyed(entity);
        }
    }
    
private:
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays;
    
    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char* typeName = typeid(T).name();
        if (mComponentArrays.find(typeName) == mComponentArrays.end()) {
            throw std::runtime_error("Component not registered before use.");
        }
        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }
};