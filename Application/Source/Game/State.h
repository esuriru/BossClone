#include "Core/Core.h"

template<class T>
class State
{
public:
    void Enter(Ref<T> owner) {}
    void Toggle(Ref<T> owner) {}
    void Exit(Ref<T> owner) {}

    inline static State& Instance()
    {
        static State instance;
        return instance; 
    }

private:
    State() = default;
    State(const State& other) = default;

};