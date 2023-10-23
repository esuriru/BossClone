#pragma once

#include "State.h"
#include "Transition.h"

#include "Core/Core.h"
#include "Core/Timestep.h"

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

        currentState_ = states_[destinationStateID].get();

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
        const auto& it = transitions_.find(stateID);
        if (it == transitions_.end())
        {
            return false;
        }

        auto& transitions = it->second;

        for (auto& transition : transitions)
        {
            if (transition->TestConditions())
            {
                transition->InvokeCallback();
                outStateID = transition->GetToID();
                return true;
            }
        }
        return false;
    }

    void HandleAllTransitions(function<void()> callback)
    {
        S outStateID;

        if (TryAllTransitions(currentState_->GetID(), outStateID))
        {
            SwitchState(outStateID, callback);
        }
    }

    void InitiateTransitionHandling(function<void()> messageCallback, 
        function<void()> callback)
    {
        if (!currentState_)
        {
            return;
        }

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
    Timestep currentTimestep;
    float fixedDeltaTime;

    StateMachine(T selfID, bool handleTransitionAfter = true)
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

    void FixedUpdate(float fixedDeltaTime)
    {
        this->fixedDeltaTime = fixedDeltaTime;

        InitiateTransitionHandling([&](){
            if (currentState_)
            {
                currentState_->FixedUpdate();
            }
        }, nullptr);
    }

    void Update(Timestep ts)
    {
        currentTimestep = ts;

        InitiateTransitionHandling([&](){
            if (currentState_)
            {
                currentState_->Update();
            }
        }, nullptr);
    }

    void InitateStartState(std::string ID)
    {
        CC_ASSERT(states_.find(ID) != states_.end(), "Starting state" 
            "could not be found.");
        currentState_ = states_[ID].get();
    }

    bool IsHandlingTransitionsAfter() const
    {
        return handleTransitionsAfter_;
    }
};