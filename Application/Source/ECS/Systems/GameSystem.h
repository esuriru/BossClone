#pragma once

#include "ECS/System.h"
#include "Core/Timestep.h"
#include "Core/Core.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/EventDispatcher.h"

#include "Core/Window.h"

struct WeaponComponent;

class WeaponSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    auto OnEvent(Event& e) -> void;

    static auto MeleeBehaviour(Entity e, WeaponUseEvent& event) -> void;
    static auto MageBehaviour(Entity e, WeaponUseEvent& event) -> void;

    static auto MeleeActiveBehaviour(Entity e) -> void;
    static auto MageActiveBehaviour(Entity e) -> void;
    
    static auto MeleeDamageCondition(WeaponComponent& wc) -> bool;

    static EventCallback eventCallback;

private:
    auto OnWeaponUseEvent(WeaponUseEvent& e) -> bool;
    auto OnCollisionEvent(CollisionEvent& e) -> bool;
};

class DamageableSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

private:
    auto OnDamageEvent(DamageEvent& e) -> bool;
    auto OnHealingEvent(HealingEvent& e) -> bool;
};

class WeaponAffectedByAnimationSystem : public System
{
public:
    auto OnEvent(Event& e) -> void;

    EventCallback eventCallback;
    static auto DefaultMeleeAnimationBehaviour(Entity e, AnimationSpriteChangeEvent& ascEvent, const std::set<size_t>& activeIndices) -> void;
private:
    auto OnItemAffectByAnimationEvent(ItemAffectByAnimationEvent& e) -> bool;

};

class PlayerAffectedByAnimationSystem : public System
{
public:
    auto OnEvent(Event& e) -> void;

    EventCallback eventCallback;
private:
    auto OnAnimationSpriteChangeEvent(AnimationSpriteChangeEvent& e) -> bool;
};

class PickupItemSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

private:
    auto OnPickupEvent(PickupEvent& e) -> bool;

};

class WeaponAffectedByPickupSystem : public System
{
public:
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<PickupEvent>(CC_BIND_EVENT_FUNC(WeaponAffectedByPickupSystem::OnPickupEvent));
    }
    static auto DefaultMeleePickupBehaviour(Entity e, PickupEvent& event) -> void; 
    static auto DefaultMagePickupBehaviour(Entity e, PickupEvent& event) -> void; 

private:
    auto OnPickupEvent(PickupEvent& e) -> bool;

};

class BreakableBoxSystem : public System
{
public:
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<DamageEvent>(CC_BIND_EVENT_FUNC(BreakableBoxSystem::OnDamageEvent));
    }

private:
    auto OnDamageEvent(DamageEvent& e) -> bool;
};

class SpikeSystem : public System 
{
public:
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<PlayerEnterEvent>(CC_BIND_EVENT_FUNC(SpikeSystem::OnPlayerEnterEvent));
    }

private:    
    auto OnPlayerEnterEvent(PlayerEnterEvent& e) -> bool;

};

class HealingPotionSystem : public System
{
public:
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WeaponUseEvent>(CC_BIND_EVENT_FUNC(HealingPotionSystem::OnWeaponUseEvent));
    }

    EventCallback eventCallback;
private:
    // TODO - Rename this.
    auto OnWeaponUseEvent(WeaponUseEvent& e) -> bool;
};


class PortalSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<PlayerEnterEvent>(CC_BIND_EVENT_FUNC(PortalSystem::OnPlayerEnterEvent));
    }

private:
    auto OnPlayerEnterEvent(PlayerEnterEvent& e) -> bool;
};

class ProjectileSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<CollisionEvent>(CC_BIND_EVENT_FUNC(ProjectileSystem::OnCollisionEvent));
        dispatcher.Dispatch<WallCollisionEvent>(CC_BIND_EVENT_FUNC(ProjectileSystem::OnWallCollisionEvent));
    }

    EventCallback eventCallback;

private:
    auto OnCollisionEvent(CollisionEvent& e) -> bool;
    auto OnWallCollisionEvent(WallCollisionEvent& e) -> bool;
};
