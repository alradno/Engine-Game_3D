#pragma once

#include "System.h"
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <typeinfo>

class SystemManager
{
public:
    template <typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        const char *typeName = typeid(T).name();
        if (mSystems.find(typeName) != mSystems.end())
        {
            throw std::runtime_error("Registering system more than once.");
        }
        auto system = std::make_shared<T>();
        mSystems[typeName] = system;
        return system;
    }

    template <typename T>
    void SetSignature(ECS::Signature signature)
    {
        const char *typeName = typeid(T).name();
        mSignatures[typeName] = signature;
    }

    void EntityDestroyed(ECS::Entity entity)
    {
        for (auto const &pair : mSystems)
        {
            pair.second->mEntities.erase(entity);
        }
    }

    void EntitySignatureChanged(ECS::Entity entity, ECS::Signature entitySignature)
    {
        for (auto const &pair : mSystems)
        {
            auto const &systemSignature = mSignatures[pair.first];
            if ((entitySignature & systemSignature) == systemSignature)
            {
                pair.second->mEntities.insert(entity);
            }
            else
            {
                pair.second->mEntities.erase(entity);
            }
        }
    }

private:
    std::unordered_map<const char *, ECS::Signature> mSignatures;
    std::unordered_map<const char *, std::shared_ptr<System>> mSystems;
};
