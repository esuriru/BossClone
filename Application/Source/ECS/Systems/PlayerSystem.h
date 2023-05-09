#pragma once
#include "ECS/System.h"
#include "Physics/PhysicsSystem.h"

#include "Events/KeyEvent.h"
#include "Core/Timestep.h"
#include "Core/Core.h"
#include "Core/Window.h"

#include <bitset>

class PlayerSystem : public System 
{
public:
    auto Update(Timestep ts) -> void;

    Ref<PhysicsSystem> physicsSystem;
    EventCallback eventCallback;
private:
    // TODO - Probably make this a bool also, waste of memory.
    std::bitset<MaxEntities> runningBitset_;

    bool mouseDown_ = false;
};