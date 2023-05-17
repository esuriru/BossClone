#pragma once
#include "ECS/System.h"

class InventoryGUISystem : public System
{
public:
    auto OnImGuiRender() -> void;

private:
};