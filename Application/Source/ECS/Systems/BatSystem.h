#include "ECS/System.h" 
#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Game/StateMachine.h"

class PlayerSystem;

class BatSystem : public System
{
public:
    BatSystem();
    void Update(Timestep ts);

    EventCallback eventCallback;
    Ref<PlayerSystem> playerSystem;

private:
    bool hasEnabledAnimation_ = false;
    StateMachine<BatSystem> stateMachine_;

};