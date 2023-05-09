#include "GameSystem.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Core/Core.h"

#include "Events/EventDispatcher.h"

EventCallback WeaponSystem::eventCallback;

static Coordinator* coordinator = Coordinator::Instance();
auto WeaponSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
          
    } 
}

auto WeaponSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WeaponUseEvent>(CC_BIND_EVENT_FUNC(WeaponSystem::OnWeaponUseEvent));

}

auto WeaponSystem::MeleeBehaviour(Entity e, WeaponUseEvent& event) -> void
{
    auto& owned_by = coordinator->GetComponent<OwnedByComponent>(e); 
    
    // Do the swing animation
    AnimationEvent animationEvent(Animation::AnimationType::Swinging, owned_by.Owner, event.IsMouseDown());
    eventCallback(animationEvent);
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
