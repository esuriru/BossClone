#pragma once

#include "State.h"
#include "TimedTransition.h"

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
    T initialID_;

    std::unordered_map<S, Scope<State<S>>> states_;
    std::unordered_map<S, vector<Scope<Transition<S>>>> transitions_;

    vector<TimedTransition<>*> timedTransitions_;

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
            ResetTimedTransitions(currentState_->GetID());
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

    void Reset()
    {
        SwitchState(initialID_, nullptr);
    }

    string GetCurrentStateName()
    {
        return currentState_ ? currentState_->GetID() : "No current state";
    }


    StateMachine* AddState(Scope<State<S>> state)
    {
        states_.insert({ state->GetID(), std::move(state) });
        return this;
    }

    StateMachine* AddTransition(Scope<Transition<S>> transition)
    {
        CC_ASSERT(states_.find(transition->GetFromID()) != states_.end() &&
            states_.find(transition->GetToID()) != states_.end(), "Transition to or from state is invalid.");

        transitions_[transition->GetFromID()].push_back(std::move(transition));
        return this;
    }

    StateMachine* AddTransition(Scope<TimedTransition<S>> transition)
    {
        CC_ASSERT(states_.find(transition->GetFromID()) != states_.end() &&
            states_.find(transition->GetToID()) != states_.end(), 
            "Timed transition to or from state is invalid.");

        auto transitionFromID = transition->GetFromID();
        auto timedTransition = transition.get();
        transitions_[transitionFromID].push_back(std::move(transition));
        timedTransitions_.push_back(timedTransition);
        return this;
    }

    const T& GetID() const
    {
        return id_;
    }

    void UpdateTimedTransitions(Timestep ts)
    {
        for (auto& transition : timedTransitions_)
        {
            if (transition->GetFromID() != currentState_->GetID())
            {
                continue;
            }
            transition->Update(ts);
        }
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
        UpdateTimedTransitions(ts);

        InitiateTransitionHandling([&](){
            if (currentState_)
            {
                currentState_->Update();
            }
        }, nullptr);
    }

    void InitateStartState(S ID)
    {
        CC_ASSERT(states_.find(ID) != states_.end(), "Starting state" 
            "could not be found.");

        initialID_ = ID;

        currentState_ = states_[ID].get();
        currentState_->Enter();

        ResetTimedTransitions(currentState_->GetID());
    }

    void ResetTimedTransitions(S ID)
    {
        for (const auto& transition : timedTransitions_)
        {
            if (transition->GetFromID() != ID)
            {
                continue;
            }
            transition->ResetTimer();
        }
    }

    bool IsHandlingTransitionsAfter() const
    {
        return handleTransitionsAfter_;
    }
};