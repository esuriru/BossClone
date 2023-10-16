#pragma once

#include "Core/Core.h"

#include "ActionEntry.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

using std::string;
using std::unordered_map;
using std::vector;
using std::function;

template<typename T = string>
class State
{
protected:
    T id_;
    unordered_map<StateMessageMethod, vector<function<void()>>> actions_;

public:
    template<class... Ts>
    State(T ID, Ts&& ... entries)
        : id_(ID)
    {
        AddAction(std::forward<Ts>(entries...));
    }

    inline const T& GetID() const
    {
        return id_;
    }

    virtual void Enter() 
    {
        for (auto& action : actions_[StateMessageMethod::Enter])
        {
            action();
        }
    }

    virtual void Update() {}
    virtual void FixedUpdate() {}
    virtual void Exit(){}

private:
    State() = default;
    State(const State& other) = default;

    bool TryParse(string messageMethodName, 
        StateMessageMethod& messageMethod)
    {
        switch (messageMethodName)
        {
            case "Enter":
                messageMethod = StateMessageMethod.Enter; 
            case "Exit":
                messageMethod = StateMessageMethod.Exit; 
            case "Update":
                messageMethod = StateMessageMethod.Update; 
                
            default:
                return false;
        }
        return true;
    }

    void AddAction(ActionEntry actionEntry)
    {
        StateMessageMethod messageMethod;
        if (TryParse(actionEntry.GetMethodName(), messageMethod))
        {
            actions_.insert({ messageMethod, actionEntry.GetAction() });
        }
    }

};