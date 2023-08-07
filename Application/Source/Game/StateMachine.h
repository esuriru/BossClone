#include "State.h"
#include "Core/Core.h"

template<typename T>
class StateMachine
{
public:
    StateMachine(T* owner) : owner_(owner) {}

    inline Ref<State<T>> GetCurrentState() const
    {
        return currentState_;
    }

    inline void SetState(State<T>& newState)
    {
        currentState_->Exit(owner_);
        currentState_ = &newState;
        currentState_->Enter(owner_);
    }

    inline void Update(Timestep ts)
    {
        if (currentState_ != nullptr)
        {
            currentState_->Update(owner_, ts);
        }
    }

protected:    
    T* owner_;
    Ref<State<T>> currentState_;

};