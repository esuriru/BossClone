#include "BatSystem.h"
#include "Events/ApplicationEvent.h"

BatSystem::BatSystem()
{
}

void BatSystem::Update(Timestep ts)
{
    Entity e = *(entities.begin());

    if (hasEnabledAnimation_)
    {
        hasEnabledAnimation_ = false;
        AnimationEvent event(Animation::AnimationType::Flying, e, true);
        eventCallback(event);
    }

    
}
