#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <set>
#include "ECS/Entity.h"
#include "ECS/Coordinator.h"

#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"

class AnimationSpriteChangeEvent;
class WeaponUseEvent;
class PickupEvent;
class PlayerEnterEvent;
class TilemapComponent;

// NOTE - Components are mainly for storing data. All the logic processes will be done in Systems.

using std::string;
using std::vector;

struct TransformComponent
{
	glm::vec3 Position{};
	glm::vec3 Rotation = glm::vec3(0.0f, 0.f, 0.f); 
	glm::vec3 Scale = { 1, 1, 1 };

	TransformComponent() = default;
	TransformComponent(const glm::vec3& pos)
		: Position(pos) {};
    
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot)
        : Position(pos), Rotation(rot) {}

    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : Position(pos), Rotation(rot), Scale(scl) {}

    glm::mat4 GetTransformMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), Position)
            * glm::mat4_cast(glm::quat(Rotation))
            * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct TagComponent
{
    string Tag;
};

struct SpriteRendererComponent
{
    glm::vec4 Colour { 1.0f, 1.0f, 1.0f, 1.0f };
    Ref<SubTexture2D> Texture = nullptr;
    float TilingFactor = 1.0f;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const glm::vec4& colour)
        : Colour(colour) {}
    SpriteRendererComponent(const Ref<SubTexture2D>& texture)
        : Texture(texture) {}
};

struct TileRendererComponent
{
    glm::vec4 Colour { 1.0f, 1.0f, 1.0f, 1.0f };
    Ref<SubTexture2D> Texture = nullptr;
    float TilingFactor = 1.0f;

    TileRendererComponent() = default;
    TileRendererComponent(const glm::vec4& colour)
        : Colour(colour) {}
    TileRendererComponent(const Ref<SubTexture2D>& texture)
        : Texture(texture) {}
};

struct Animation
{
    enum AnimationType : uint16_t
    {
        None = 0,
        Running,
        Swinging,
    };
    vector<size_t> AnimationIndices;
    vector<Ref<SubTexture2D>> SpriteTextures;
    uint32_t FramesBetweenTransition = 8;
};

struct RunningAnimationComponent 
{
    RunningAnimationComponent() = default;

    Animation Animation;  
    bool Enabled = false;
};

struct SwingingAnimationComponent 
{
    SwingingAnimationComponent() = default;

    Animation Animation;  
    bool Enabled = false;
};

// NOTE - For this game there are only 6 slots.
constexpr size_t InventorySize = 6;
struct InventoryComponent
{
    InventoryComponent()
    {
        Items.reserve(InventorySize);
    }

    std::vector<Entity> Items;
    Entity CurrentlyHolding;
};

struct DescriptionComponent
{
    string Description;
};

enum class WeaponType
{
    Melee,
    Ranged,
};

struct WeaponComponent;
using WeaponBehaviour = void(*)(Entity, WeaponUseEvent&);
struct WeaponComponent
{
    float Damage = 0.f;
    WeaponBehaviour Behaviour;  

    // For collision
    glm::vec2 GroundExtents{};
    glm::vec2 EquippedExtents{};
    glm::vec2 HandOffset{};

    bool Active = false;
    bool HiddenOnActive = true;

};

struct OwnedByComponent
{
    OwnedByComponent() = default;
    OwnedByComponent(Entity owner) : Owner(owner) {}

    Entity Owner;
};

struct HealthComponent
{
    using OnDeathCallback = void(*)(Entity);
    using OnIFrameCallback = OnDeathCallback; 
    using OnIFrameEndCallback = OnDeathCallback;

    HealthComponent() = default;
    HealthComponent(float max, uint32_t cooldownFrames = 0, OnDeathCallback odCallback = [](Entity){},
        OnIFrameCallback ifCallback = [](Entity){}, OnIFrameEndCallback ifeCallback = [](Entity){}, bool reset = true)
        : MaxHealth(max)
        , CooldownFramesOnHit(cooldownFrames)
        , OnDeathBehaviour(odCallback)
        , OnIFrameBehaviour(ifCallback)
        , OnIFrameEndBehaviour(ifeCallback)
    {
        if (reset)
            Health = MaxHealth;
    }

    float Health = 0.f;
    float MaxHealth = 100.f;

    uint32_t CurrentCooldownFrames = 0;
    uint32_t CooldownFramesOnHit = 60;

    OnDeathCallback OnDeathBehaviour;
    OnIFrameCallback OnIFrameBehaviour;
    OnIFrameEndCallback OnIFrameEndBehaviour;

};

struct AffectedByAnimationComponent
{
    AffectedByAnimationComponent() = default;
    AffectedByAnimationComponent(uint16_t types) : TargetAnimationTypes(types) {}

    uint16_t TargetAnimationTypes; 
};

struct WeaponAffectedByAnimationComponent
{
    using WeaponAnimationBehaviour = void(*)(Entity, AnimationSpriteChangeEvent&, const std::set<size_t>&);
    WeaponAffectedByAnimationComponent() = default;
    WeaponAffectedByAnimationComponent(WeaponAnimationBehaviour wab) : AnimationBehaviour(wab) {}

    WeaponAnimationBehaviour AnimationBehaviour;
};

struct WeaponAffectedByPickupComponent
{
    using WeaponPickupBehaviour = void(*)(Entity, PickupEvent&);
    WeaponAffectedByPickupComponent() = default;
    WeaponAffectedByPickupComponent(WeaponPickupBehaviour wpb)
        : PickupBehaviour(wpb) {}

    WeaponPickupBehaviour PickupBehaviour;    
};

struct ItemComponent 
{
    ItemComponent() = default;
    ItemComponent(const Ref<SubTexture2D>& icon) : Icon(icon) {}

    Ref<SubTexture2D> Icon;
};

struct PickupComponent {};

// NOTE - MUST HAVE HEALTH COMPONENT
// NOTE - There will be a system with this component that intercepts the damage event such that the system
// NOTE - with the breakable component handles it first.
struct BreakableComponent
{
    using BreakBehaviourCallback = void(*)(Entity);
    BreakBehaviourCallback OnBreakBehaviour = [](Entity e)
    {
        static Coordinator* coordinator = Coordinator::Instance();
        coordinator->GetComponent<HealthComponent>(e).OnDeathBehaviour(e);
    };

    float MinimumDamageToRegister = 0;

    BreakableComponent() = default;
    BreakableComponent(BreakBehaviourCallback obb, float mdtr = 0)
        : OnBreakBehaviour(obb), MinimumDamageToRegister(mdtr) {}
    BreakableComponent(float mdtr)
        : MinimumDamageToRegister(mdtr) {}
};

struct SpikeComponent
{
    SpikeComponent() = default;
    SpikeComponent(float damage) : Damage(damage) {}

    float Damage = 5.f;
};

struct HealthPotionComponent
{
    HealthPotionComponent() = default;
    HealthPotionComponent(float healing) : Healing(healing) {}

    float Healing = 50.f;
};

struct PortalComponent
{
    using PortalBehaviour = void(*)(Entity, PlayerEnterEvent&);

    PortalComponent() = default;
    PortalComponent(PortalBehaviour behaviour) : Behaviour(behaviour) {}

    PortalBehaviour Behaviour;
};

struct ReferenceComponent 
{
    ReferenceComponent() = default;
    ReferenceComponent(Entity entity) : RefEntity(entity) {}
    Entity RefEntity;
};