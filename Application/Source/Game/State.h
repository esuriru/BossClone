#include "Core/Core.h"
#include "Core/Timestep.h"

template<class T>
class State
{
public:
    virtual void Enter(T* owner) {}
    virtual void Toggle(T* owner) {}
    virtual void Update(T* owner, Timestep ts) {}
    virtual void Exit(T* owner) {}

    inline static State& Instance()
    {
        static State instance;
        return instance; 
    }

private:
    State() = default; 
    State(const State& other) = default;
};