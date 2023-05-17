#include "GameSystem.h"

#include "Physics/PhysicsComponent.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Core/Core.h"

#include "Events/EventDispatcher.h"
#include "Events/ApplicationEvent.h"

EventCallback WeaponSystem::eventCallback;

static Coordinator* coordinator = Coordinator::Instance();
auto WeaponSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& weapon = coordinator->GetComponent<WeaponComponent>(e);
        if (weapon.Active)
        {
            auto& transform = coordinator->GetComponent<TransformComponent>(e);
            auto& owned_by = coordinator->GetComponent<OwnedByComponent>(e);
            auto& owner_centre = coordinator->GetComponent<TransformComponent>(owned_by.Owner).Position;
            auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);

            bool ownerIsFacingRight = coordinator->GetComponent<PlayerController2DComponent>(owned_by.Owner).IsFacingRight;

            // For detecting collision.
            transform.Position = owner_centre +
                glm::vec3((-1.f + 2.f * static_cast<float>(ownerIsFacingRight)) * weapon.HandOffset.x, weapon.HandOffset.y, 0.f);
        }
    } 
}

auto WeaponSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WeaponUseEvent>(CC_BIND_EVENT_FUNC(WeaponSystem::OnWeaponUseEvent));
    dispatcher.Dispatch<CollisionEvent>(CC_BIND_EVENT_FUNC(WeaponSystem::OnCollisionEvent));

}

auto WeaponSystem::MeleeBehaviour(Entity e, WeaponUseEvent& event) -> void
{
    auto& owned_by = coordinator->GetComponent<OwnedByComponent>(e); 
    
    // Do the swing animation
    AnimationEvent animationEvent(Animation::AnimationType::Swinging, owned_by.Owner, event.IsMouseDown());
    eventCallback(animationEvent);

    // Change the collider
    auto& weapon = coordinator->GetComponent<WeaponComponent>(e);
    auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);
    weapon.Active = event.IsMouseDown();
    if (!weapon.Active)
        physics_quadtree.Active = false;
}

auto WeaponSystem::OnWeaponUseEvent(WeaponUseEvent &e) -> bool 
{
    Entity weaponEntity = e.GetWeapon();

    if (entities.find(weaponEntity) == entities.end())
    {
        return false;
    }

    auto& weapon = coordinator->GetComponent<WeaponComponent>(weaponEntity);
    weapon.Behaviour(weaponEntity, e);
    return true;
}

auto WeaponSystem::OnCollisionEvent(CollisionEvent &e) -> bool
{
    const auto& collision = e.GetCollision();

    bool originWeapon = false;
    Entity weaponEntity;

    auto otherEntityQuery = entities.find(collision.OtherEntity);
    auto originEntityQuery = entities.find(e.GetOriginEntity());
    if (otherEntityQuery == entities.end())
    {
        if (originEntityQuery == entities.end())
            return false; 
        else
            originWeapon = true;
    }
    else
    {
        if (originEntityQuery == entities.end())
            originWeapon = false;
        else 
            return false;
    }

    Entity targetEntity;
    if (originWeapon)
    {
        weaponEntity = e.GetOriginEntity();
        targetEntity = collision.OtherEntity;
    }
    else
    {
        weaponEntity = collision.OtherEntity;
        targetEntity = e.GetOriginEntity();
    }
    auto& weaponComponent = coordinator->GetComponent<WeaponComponent>(weaponEntity);

    // Send a damage event.
    DamageEvent damageEvent(weaponEntity, weaponComponent, targetEntity);
    eventCallback(damageEvent);

    return true;
}

auto DamageableSystem::Update(Timestep ts) -> void
{
    constexpr float step = 1 / 50.f;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step)
    {
        for (auto& e : entities)
        {
            auto& health = coordinator->GetComponent<HealthComponent>(e);

            // NOTE - Decrement in if statement
            if (health.CurrentCooldownFrames > 0 && --health.CurrentCooldownFrames == 0)
                coordinator->GetComponent<PhysicsQuadtreeComponent>(e).Active = true;

        }
        accumulator -= step;
    }
}

auto DamageableSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<DamageEvent>(CC_BIND_EVENT_FUNC(DamageableSystem::OnDamageEvent));
}

auto DamageableSystem::OnDamageEvent(DamageEvent &e) -> bool
{
    Entity target = e.GetTargetEntity();
    if (entities.find(target) == entities.end())
        return false;

    auto& health = coordinator->GetComponent<HealthComponent>(target);
    health.Health -= e.GetWeaponComponent().Damage;

    // Apply an i-frame to the enemy.
    health.CurrentCooldownFrames += health.CooldownFramesOnHit;
    auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(target);
    // CC_ASSERT(!physics_quadtree.Active, "Break");
    physics_quadtree.Active = false;

    CC_TRACE("New health: ", health.Health);

    return true;
}

auto WeaponAffectedByAnimationSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<ItemAffectByAnimationEvent>(CC_BIND_EVENT_FUNC(WeaponAffectedByAnimationSystem::OnItemAffectByAnimationEvent));
}

auto WeaponAffectedByAnimationSystem::OnItemAffectByAnimationEvent(ItemAffectByAnimationEvent &e) -> bool
{
    if (entities.find(e.GetItemEntity()) == entities.end())
        return false;
            
    auto& waba = coordinator->GetComponent<WeaponAffectedByAnimationComponent>(e.GetItemEntity()); 

    waba.AnimationBehaviour(e.GetItemEntity(), e.GetAnimationSpriteChangeEvent(),
        coordinator->GetComponent<PlayerController2DComponent>(e.GetAnimationSpriteChangeEvent().
            GetEntityAffected()).ActiveMeleeWeaponIndices);

    return true;
}


auto PlayerAffectedByAnimationSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<AnimationSpriteChangeEvent>(CC_BIND_EVENT_FUNC(PlayerAffectedByAnimationSystem::OnAnimationSpriteChangeEvent));
}

auto PlayerAffectedByAnimationSystem::OnAnimationSpriteChangeEvent(AnimationSpriteChangeEvent &e) -> bool 
{
    // NOTE - Realistically, if there is only one player, this is pointless, however, this is just in case.
    if (entities.find(e.GetEntityAffected()) == entities.end())
        return false;

    auto& animation_effect = coordinator->GetComponent<AffectedByAnimationComponent>(e.GetEntityAffected());
    if (!(animation_effect.TargetAnimationTypes & e.GetAnimationType()))
        return false;

    auto& inventory = coordinator->GetComponent<InventoryComponent>(e.GetEntityAffected());

    // Create an event with what the player has in his hand.
    auto event = ItemAffectByAnimationEvent(inventory.CurrentlyHolding, e);
    eventCallback(event);
    
    return true;
}
