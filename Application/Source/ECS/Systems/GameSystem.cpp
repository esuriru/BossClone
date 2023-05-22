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
    constexpr float step = 1 / CC_FIXED_UPDATE_FRAME_RATE;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step)
    {
        for (auto& e : entities)
        {
            auto& health = coordinator->GetComponent<HealthComponent>(e);

            // NOTE - Decrement in if statement
            if (health.CurrentCooldownFrames > 0 && --health.CurrentCooldownFrames == 0)
            {
                // TODO - A callback for when the iframe ends, i guess.
                // coordinator->GetComponent<PhysicsQuadtreeComponent>(e).Active = true;
                health.OnIFrameEndBehaviour(e);
            }
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
    if (entities.find(target) == entities.end() || !e.GetWeaponComponent().Active ||
        coordinator->GetComponent<OwnedByComponent>(e.GetWeaponEntity()).Owner == target) return false;

    auto& health = coordinator->GetComponent<HealthComponent>(target);
    if (health.CurrentCooldownFrames > 0)
        return true;

    health.Health -= e.GetWeaponComponent().Damage;

    // Apply an i-frame to the enemy.
    health.CurrentCooldownFrames += health.CooldownFramesOnHit;
    health.OnIFrameBehaviour(target);
    // auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(target);
#if _DEBUG
    coordinator->GetComponent<SpriteRendererComponent>(target).Colour = { 1.0f, 0.f, 0.f , 1.0f };
#endif
    // physics_quadtree.Active = false;

    CC_TRACE("New health: ", health.Health);

    return true;
}

auto WeaponAffectedByAnimationSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<ItemAffectByAnimationEvent>(CC_BIND_EVENT_FUNC(WeaponAffectedByAnimationSystem::OnItemAffectByAnimationEvent));
}

auto WeaponAffectedByAnimationSystem::DefaultMeleeAnimationBehaviour(Entity e, AnimationSpriteChangeEvent &ascEvent, const std::set<size_t> &activeIndices) -> void
{
    bool indexIsActive = activeIndices.find(ascEvent.GetSpriteIndex()) != activeIndices.end();
    coordinator->GetComponent<PhysicsQuadtreeComponent>(e).Active = indexIsActive;
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

auto PickupItemSystem::Update(Timestep ts) -> void
{
}

auto PickupItemSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<PickupEvent>(CC_BIND_EVENT_FUNC(PickupItemSystem::OnPickupEvent));
}

auto PickupItemSystem::OnPickupEvent(PickupEvent &e) -> bool
{
    if (entities.find(e.GetTargetEntity()) == entities.end())
        return false;

    auto& inventory = coordinator->GetComponent<InventoryComponent>(e.GetPlayerEntity());
    if (inventory.Items.size() < InventorySize)
        inventory.Items.emplace_back(e.GetTargetEntity());
    else
        return true;

    for (auto& item : inventory.Items)
    {
        CC_TRACE(item);
    }
    CC_TRACE("Currently holding: ", inventory.CurrentlyHolding);

    e.IsPickedUp() = true;
    // TODO - Could have some issues where the player can hit things he owns.
    coordinator->AddComponent(e.GetTargetEntity(), OwnedByComponent(e.GetPlayerEntity()));

    // Remove the item from being able to be picked up.
    coordinator->RemoveComponent<PickupComponent>(e.GetTargetEntity());
    coordinator->RemoveComponent<SpriteRendererComponent>(e.GetTargetEntity());
    auto& physics_obj = coordinator->GetComponent<PhysicsQuadtreeComponent>(e.GetTargetEntity());
    physics_obj.Active = false;

    return false;
}

auto WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour(Entity e, PickupEvent &event) -> void
{
    auto& weapon_component = coordinator->GetComponent<WeaponComponent>(e);
    // NOTE - Every weapon should have a box collider, if it does not, how would it have been picked up anyway?
    auto& box_collider = coordinator->GetComponent<BoxCollider2DComponent>(e);

    // Set the new extents to those of the correct use case when being held by the player.
    box_collider.Extents = weapon_component.EquippedExtents;
}

auto WeaponAffectedByPickupSystem::OnPickupEvent(PickupEvent &e) -> bool
{
    Entity weaponEntity = e.GetTargetEntity();
    if (e.IsPickedUp()) 
    {
        auto& weaponAffectedByPickupComponent = coordinator->GetComponent<WeaponAffectedByPickupComponent>(weaponEntity);
        weaponAffectedByPickupComponent.PickupBehaviour(weaponEntity, e);
    }
    return true;
}

auto BreakableBoxSystem::OnDamageEvent(DamageEvent &e) -> bool
{
    Entity target = e.GetTargetEntity();
    if (entities.find(target) == entities.end() || !e.GetWeaponComponent().Active ||
        coordinator->GetComponent<OwnedByComponent>(e.GetWeaponEntity()).Owner == target) return false;

    auto& breakable = coordinator->GetComponent<BreakableComponent>(target);

    if (e.GetWeaponComponent().Damage < breakable.MinimumDamageToRegister)
        // NOTE - Should this return as true?
        return true;

    auto& health = coordinator->GetComponent<HealthComponent>(target);
    if (health.CurrentCooldownFrames > 0)
        return true;

    health.Health -= e.GetWeaponComponent().Damage;

    // Apply an i-frame to the enemy.
    health.CurrentCooldownFrames += health.CooldownFramesOnHit;
    health.OnIFrameBehaviour(target);
    // auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(target);
#if _DEBUG
    auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(target);
    // physics_quadtree.Active = false;
    if (health.Health <= 0)
    {
        breakable.OnBreakBehaviour(target);
    }
#endif
    // NOTE - DO NOT HAVE ANYTHING AFTER THIS CAUSE THERE IS A POSSIBILITY THAT THIS ENTITY IS DESTROYED.

    return true;
}

auto SpikeSystem::OnPlayerEnterEvent(PlayerEnterEvent &e) -> bool
{
    if (entities.find(e.GetTargetEntity()) == entities.end())
        return false;

    auto& player_health = coordinator->GetComponent<HealthComponent>(e.GetPlayerEntity());
    if (player_health.CurrentCooldownFrames > 0)
        return false;

    auto& spike = coordinator->GetComponent<SpikeComponent>(e.GetTargetEntity());

    player_health.Health -= spike.Damage;
    player_health.CurrentCooldownFrames += player_health.CooldownFramesOnHit;
    player_health.OnIFrameBehaviour(e.GetPlayerEntity());

    // TODO - Fix the health system such that the player can still detect pickups while not taking damage.
    // auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(e.GetPlayerEntity());
    // physics_quadtree.Active = false;

    if (player_health.Health <= 0)
    {
        player_health.OnDeathBehaviour(e.GetPlayerEntity());
    }
    
    return false;
}
