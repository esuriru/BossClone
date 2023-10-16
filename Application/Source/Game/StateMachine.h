#pragma once

#include "State.h"
#include "Transition.h"
#include "Core/Core.h"

#include <unordered_map>
#include <string>
#include <vector>

using std::string;
using std::vector;

template<typename T = string, typename S = string>
class StateMachine
{
protected:
    T id_;
    bool handleTransitionsAfter_;

    std::unordered_map<S, Scope<State<S>>> states_;
    std::unordered_map<S, vector<Scope<Transition<S>>>> transitions_;

    State<S>* currentState_;

    void SwitchState(S destinationStateID,
        function<void()> callback)
    {
        if (currentState_)
        {
            currentState_->Exit();
        }

        currentState_ = states_[destinationStateID];

        if (currentState_)
        {
            currentState_->Enter();
        }
    
        if (callback)
        {
            callback();
        }
    }


    bool TryAllTransitions(S stateID, S& outStateID)
    {
        auto& it = transitions_.find(stateID);
        if (it == transitions_.end())
        {
            return false;
        }

        auto& transitions = it.second;

        for (auto& transition : transitions)
        {
            if (transition->TestConditions())
            {
                transition->TriggerCallback();
                outStateID = transition.GetToID();
                return true;
            }
        }
    }

    void HandleAllTransitions(function<void()> callback)
    {
        S outStateID;

        if (TryAllTransitions(outStateID))
        {
            SwitchState(outStateID, callback);
        }
    }

    void InitiateTransitionHandling(function<void()> messageCallback, 
        function<void()> callback)
    {
        if (handleTransitionsAfter_)
        {
            messageCallback();
            HandleAllTransitions(callback);
        }
        else
        {
            HandleAllTransitions(callback);
            messageCallback();
        }
    }

public:
    StateMachine(T selfID, bool handleTransitionAfter)
        : id_(selfID) 
        , handleTransitionsAfter_(handleTransitionAfter)
    {

    }

    StateMachine* AddState(Scope<State<S>> state)
    {
        states_.insert({ state->GetID(), std::move(state) });
        return this;
    }

    StateMachine* AddTransition(Scope<Transition<S>> transition)
    {
        transitions_.insert({ transition->GetID(), std::move(transition) });
        return this;
    }

    const T& GetID() const
    {
        return id_;
    }

    void Update()
    {
        currentState_->Update();
        InitiateTransitionHandling([](){
            Update(); 
        }, nullptr);
    }

    bool IsHandlingTransitionsAfter() const
    {
        return handleTransitionsAfter_;
    }
};