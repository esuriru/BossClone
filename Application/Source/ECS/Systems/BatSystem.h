#include "ECS/System.h" 
#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Game/StateMachine.h"
#include "Core/Core.h"
#include "Events/EventDispatcher.h"
#include "Events/ApplicationEvent.h"

class PlayerSystem;

class BatSystem : public System
{
public:
    BatSystem();
    void Update(Timestep ts);

    EventCallback eventCallback;
    Ref<PlayerSystem> playerSystem;

    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<PlayerEnterEvent>(CC_BIND_EVENT_FUNC(BatSystem::OnPlayerEnterEvent));
    }

private:
    auto OnPlayerEnterEvent(PlayerEnterEvent& e) -> bool;

    void StateUpdate(Timestep ts);
    enum BatState
    {
        IDLE,
        PATROL,
        CHASE,
    };

    BatState currentState_;

    bool hasEnabledAnimation_ = false;

};