#pragma once

#include "Event.h"
#include "ECS/Entity.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ECS/Component.h"

#include "Physics/Collision2D.h"

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int w, unsigned int h) :  width_(w), height_(h) {}

	inline unsigned int GetWidth() const
	{
		return width_;
	}

	inline unsigned int GetHeight() const
	{
		return height_;
	}

	inline glm::vec2 GetSize() const
	{
		return { width_, height_ };
	}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << width_ << " x " << height_;
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(WindowResize)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
	unsigned int width_, height_;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	NR_EVENT_CLASS_TYPE(WindowClose)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppTickEvent : public Event
{
public:
	AppTickEvent() = default;

	NR_EVENT_CLASS_TYPE(AppTick)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() = default;

	NR_EVENT_CLASS_TYPE(AppUpdate)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppRenderEvent : public Event
{
public:
	AppRenderEvent() = default;

	NR_EVENT_CLASS_TYPE(AppRender)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AnimationEvent : public Event
{
public:
    AnimationEvent(Animation::AnimationType type, Entity entity, bool animationEnabled) : 
        animationEnabled_(animationEnabled), animationType_(type), entityAffected_(entity) {}

	NR_EVENT_CLASS_TYPE(Animation)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)

    inline auto IsAnimationEnabled() -> bool const { return animationEnabled_; }
    inline auto GetEntityAffected() -> Entity const { return entityAffected_; }
    inline auto GetAnimationType() -> Animation::AnimationType const { return animationType_; }

private:
    Animation::AnimationType animationType_;
    bool animationEnabled_;
    Entity entityAffected_;
};

class AnimationSpriteChangeEvent : public Event
{
public:
    AnimationSpriteChangeEvent(Animation::AnimationType type, Entity entity, size_t index)
        : animationType_(type), entityAffected_(entity), spriteIndex_(index) {}

	NR_EVENT_CLASS_TYPE(AnimationSpriteChange)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)

    inline auto GetSpriteIndex() -> size_t const { return spriteIndex_; }
    inline auto GetEntityAffected() -> Entity const { return entityAffected_; }
    inline auto GetAnimationType() -> Animation::AnimationType const { return animationType_; }
private:
    Animation::AnimationType animationType_;
    Entity entityAffected_;
    size_t spriteIndex_;
};

class WeaponUseEvent : public Event
{
public:
    WeaponUseEvent(Entity owner, Entity item, bool mouseDown, bool facingRight)
       : owner_(owner), weapon_(item), mouseDown_(mouseDown), facingRight_(facingRight){}

    NR_EVENT_CLASS_TYPE(WeaponUse)
    NR_EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryInput)

    inline auto GetOwner() -> Entity const { return owner_; }
    inline auto GetWeapon() -> Entity const { return weapon_; }
    inline auto IsMouseDown() -> bool const { return mouseDown_; }
    inline auto IsFacingRight() -> bool const { return facingRight_; }

private:
    Entity owner_;
    Entity weapon_;
    bool mouseDown_;
    bool facingRight_;
};

class CollisionEvent : public Event
{
public:
    CollisionEvent(Entity origin, Collision2D& collision) : origin_(origin), collision_(collision) {}

    NR_EVENT_CLASS_TYPE(Collision)
    NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)

    inline auto GetCollision() -> const Collision2D& { return collision_; }
    inline auto GetOriginEntity() -> Entity& { return origin_; }

private:
    Entity origin_;
    Collision2D& collision_;
};

class DamageEvent : public Event
{
public:
    DamageEvent(Entity weaponEntity, WeaponComponent& weaponComponent, Entity targetEntity) :
        weaponEntity_(weaponEntity), weaponComponent_(weaponComponent), targetEntity_(targetEntity) {}

    NR_EVENT_CLASS_TYPE(Damage)
    NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)

    inline auto GetWeaponEntity() -> Entity { return weaponEntity_; } 
    inline auto GetTargetEntity() -> Entity { return targetEntity_; } 
    inline auto GetWeaponComponent() -> WeaponComponent& { return weaponComponent_; }

private:
    Entity weaponEntity_, targetEntity_;
    WeaponComponent& weaponComponent_;

};

// NOTE - This event is for affecting items (mainly what the character is holding)
// NOTE - Mainly for setting the collider of the melee weapon active during correct timings.
class ItemAffectByAnimationEvent : public Event
{
public:
    ItemAffectByAnimationEvent(Entity itemEntity, AnimationSpriteChangeEvent& ascEvent)
        : itemEntity_(itemEntity), animationSpriteChangeEvent(ascEvent) {}

    inline auto GetItemEntity() -> Entity { return itemEntity_; } 
    inline auto GetAnimationSpriteChangeEvent() -> AnimationSpriteChangeEvent& { return animationSpriteChangeEvent; }

    NR_EVENT_CLASS_TYPE(ItemAffectByAnimation)
    NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
    Entity itemEntity_;
    AnimationSpriteChangeEvent& animationSpriteChangeEvent;

};
