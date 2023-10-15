#pragma once

#include "State.h"
#include "Core/Core.h"

#include <unordered_map>
#include <string>

using std::string;

template<typename T = string, typename S = string>
class StateMachine
{
protected:
    T id_;
    bool handleTransitionsAfter_;

    std::unordered_map<S, Scope<State<S>>> states_;
    State<S>* currentState_;

public:
    template<class... Ts>
    StateMachine(T selfID, Ts ... states)
        : id_(selfID) 
        , handleTransitionsAfter_(true)
    {
        AddState(std::forward<Ts>(states...));
    }

    void AddState(Scope<State<S>> state)
    {
        states_.insert({ state->GetID(), std::move(state) });
    }

    const T& GetID() const
    {
        return id_;
    }

    void Update()
    {
        currentState_->Update();
    }

    bool IsHandlingTransitionsAfter() const
    {
        return handleTransitionsAfter_;
    }
};