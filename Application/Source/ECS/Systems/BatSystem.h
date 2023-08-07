#include "ECS/System.h" 
#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Game/StateMachine.h"

class BatSystem : public System
{
public:
    BatSystem();
    void Update(Timestep ts);

    EventCallback eventCallback;
    StateMachine<BatSystem> stateMachine_;
private:
    bool hasEnabledAnimation_ = false;
};