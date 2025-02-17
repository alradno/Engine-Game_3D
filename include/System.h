#pragma once

#include "ECS.h"
#include <set>

class System
{
public:
    std::set<ECS::Entity> mEntities;
};
