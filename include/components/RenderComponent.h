#pragma once

#include <memory>
#include "renderer/Model.h"

struct RenderComponent {
    std::shared_ptr<Model> model;
};
