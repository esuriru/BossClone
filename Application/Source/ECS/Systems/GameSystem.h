#pragma once

#include "ECS/System.h"
#include "Core/Timestep.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Core/Window.h"

struct WeaponComponent;

class WeaponSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    auto OnEvent(Event& e) -> void;

    static auto MeleeBehaviour(Entity e, WeaponUseEvent& event) -> void;
    static EventCallback eventCallback;

private:
    auto OnWeaponUseEvent(WeaponUseEvent& e) -> bool;
    auto OnCollisionEvent(CollisionEvent& e) -> bool;
};

class DamageableSystem : public System
{
public:
    auto OnEvent(Event& e) -> void;

private:
    auto OnDamageEvent(DamageEvent& e) -> bool;
};
