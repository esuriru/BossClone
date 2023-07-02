#include "State.h"
#include "Core/Core.h"

template<typename T>
class StateMachine
{
public:
    StateMachine() = default;
    inline Ref<State<T>> GetCurrentState() const
    {
        return currentState_;
    }

    inline void SetState(State<T>& newState)
    {
        currentState_->Exit(this);
        currentState = &newState;
        currentState->Enter(this);
    }

protected:    
    Ref<State<T>> currentState_;

};