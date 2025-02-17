#pragma once

#include "ECS.h"
#include <queue>
#include <array>
#include <stdexcept>

class EntityManager
{
public:
    EntityManager() : mLivingEntityCount(0)
    {
        for (ECS::Entity entity = 0; entity < ECS::MAX_ENTITIES; ++entity)
        {
            mAvailableEntities.push(entity);
        }
    }

    ECS::Entity CreateEntity()
    {
        if (mLivingEntityCount >= ECS::MAX_ENTITIES)
        {
            throw std::runtime_error("Too many entities in existence.");
        }
        ECS::Entity id = mAvailableEntities.front();
        mAvailableEntities.pop();
        mLivingEntityCount++;
        return id;
    }

    void DestroyEntity(ECS::Entity entity)
    {
        mSignatures[entity].reset();
        mAvailableEntities.push(entity);
        mLivingEntityCount--;
    }

    void SetSignature(ECS::Entity entity, ECS::Signature signature)
    {
        mSignatures[entity] = signature;
    }

    ECS::Signature GetSignature(ECS::Entity entity)
    {
        return mSignatures[entity];
    }

private:
    std::queue<ECS::Entity> mAvailableEntities;
    std::array<ECS::Signature, ECS::MAX_ENTITIES> mSignatures;
    uint32_t mLivingEntityCount;
};
