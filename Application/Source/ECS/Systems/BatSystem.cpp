#include "BatSystem.h"
#include "Events/ApplicationEvent.h"
#include "Core/Log.h"

BatSystem::BatSystem()
    : stateMachine_(this) 
{
}

void BatSystem::Update(Timestep ts)
{
    Entity e = *(entities.begin());

    if (!hasEnabledAnimation_)
    {
        hasEnabledAnimation_ = true;
        AnimationEvent event(Animation::AnimationType::Flying, e, true);
        eventCallback(event);
    }

    stateMachine_.Update(ts);
}
