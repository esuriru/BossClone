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

            // For detecting collision.
            transform.Position = owner_centre + glm::vec3(weapon.HandOffset, 0);
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
    physics_quadtree.Active = weapon.Active = event.IsMouseDown();

    // if (event.IsMouseDown())
    // {
    //     // auto& box_collider = coordinator->GetComponent<BoxCollider2DComponent>(e);
    //     // auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);

    //     // rigidbody.BodyType = Physics::RigidBodyType::Static;
    //     physics_quadtree.Active = weapon.Active = true;
    // }
    // else
    // {
    //     // While the person is not swinging, we don't want the weapon to detect it being hit.
    //     physics_quadtree.Active = weapon.Active = false;
    // }
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
    CC_TRACE("New health: ", health.Health);

    return true;
}
