#pragma once

#include <memory>
#include "Model.h"

struct RenderComponent {
    std::shared_ptr<Model> model;
};
