// ComponentManager.h
#pragma once

#include "ECS.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>
#include <optional>
#include <stdexcept>

// Interfaz base para arrays de componentes
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(ECS::Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray() {
        data.resize(ECS::MAX_ENTITIES);
    }
    
    void InsertData(ECS::Entity entity, T component) {
        data[entity] = component;
    }
    
    void RemoveData(ECS::Entity entity) {
        data[entity].reset();
    }
    
    T& GetData(ECS::Entity entity) {
        if (!data[entity].has_value())
            throw std::runtime_error("Component not found for entity " + std::to_string(entity));
        return data[entity].value();
    }
    
    bool HasData(ECS::Entity entity) {
        return data[entity].has_value();
    }
    
    void EntityDestroyed(ECS::Entity entity) override {
        data[entity].reset();
    }
private:
    std::vector<std::optional<T>> data;
};

class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        std::type_index typeIndex(typeid(T));
        if(componentArrays.find(typeIndex) != componentArrays.end())
            throw std::runtime_error("Registering component type more than once.");
        componentArrays[typeIndex] = std::make_shared<ComponentArray<T>>();
    }
    
    template<typename T>
    void AddComponent(ECS::Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }
    
    template<typename T>
    void RemoveComponent(ECS::Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }
    
    template<typename T>
    T& GetComponent(ECS::Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }
    
    void EntityDestroyed(ECS::Entity entity) {
        for(auto const& pair : componentArrays) {
            pair.second->EntityDestroyed(entity);
        }
    }
    
private:
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays;
    
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        std::type_index typeIndex(typeid(T));
        if(componentArrays.find(typeIndex) == componentArrays.end())
            throw std::runtime_error("Component not registered before use.");
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeIndex]);
    }
};