#pragma once

#include <cstdint>
#include <bitset>

namespace ECS
{
    using Entity = uint32_t;
    const Entity MAX_ENTITIES = 5000;

    using ComponentType = uint8_t;
    const ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

    // Get a new unique ComponentType ID for each component type
    inline ComponentType GetNewComponentTypeID()
    {
        static ComponentType lastID = 0u;
        return lastID++;
    }

    template <typename T>
    inline ComponentType GetComponentTypeID() noexcept
    {
        static ComponentType typeID = GetNewComponentTypeID();
        return typeID;
    }
}