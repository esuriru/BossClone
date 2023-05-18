#pragma once
#include "ECS/System.h"
#include "Physics/PhysicsSystem.h"

#include "Events/KeyEvent.h"
#include "Core/Timestep.h"
#include "Core/Core.h"
#include "Core/Window.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include <bitset>

class PlayerSystem : public System 
{
public:
    PlayerSystem();

    auto Update(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

    Ref<PhysicsSystem> physicsSystem;
    EventCallback eventCallback;
private:
    // TODO - Probably make this a bool also, waste of memory.
    std::bitset<MaxEntities> runningBitset_;

    auto OnCollisionEvent(CollisionEvent& e) -> bool;
    auto OnKeyPressedEvent(KeyPressedEvent& e) -> bool;

    bool mouseDown_ = false;
};