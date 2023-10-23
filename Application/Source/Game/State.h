#pragma once

#include "Core/Core.h"
#include "Core/Timestep.h"

#include "ActionEntry.h"
#include "Utils/Util.h"

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
    unordered_map<MessageMethod, vector<function<void()>>> actions_;

public:
    template<class... Ts>
    State(T ID, Ts&& ... entries)
        : id_(ID)
    {
        for (const auto& entry : {entries...})
        {
            AddAction(entry);
        }
    }

    State(T ID) : id_(ID) {}

    inline const T& GetID() const
    {
        return id_;
    }

    void AddAction() {}

    State* AddAction(ActionEntry actionEntry)
    {
        MessageMethod messageMethod;
        if (TryParse(actionEntry.GetMethodName(), messageMethod))
        {
            actions_[messageMethod].emplace_back(actionEntry.GetAction());
        }
        return this;
    }

    virtual void Enter() 
    {
        for (auto& action : actions_[MessageMethod::Enter])
        {
            action();
        }
    }


    virtual void Update() 
    {
        for (auto& action : actions_[MessageMethod::Update])
        {
            action();
        }
    }

    virtual void FixedUpdate(float fixedDeltaTime) 
    {
        for (auto& action : actions_[MessageMethod::FixedUpdate])
        {
            action();
        }
    }

    virtual void Exit()
    {
        for (auto& action : actions_[MessageMethod::Exit])
        {
            action();
        }
    }

private:
    State() = default;
    State(const State& other) = default;

    bool TryParse(string messageMethodName, 
        MessageMethod& messageMethod)
    {
        if (messageMethodName == "Enter")
        {
            messageMethod = MessageMethod::Enter;
        }
        else if (messageMethodName == "Exit")
        {
            messageMethod = MessageMethod::Exit;
        }
        else if (messageMethodName == "Update")
        {
            messageMethod = MessageMethod::Update;
        }
        else if (messageMethodName == "FixedUpdate")
        {
            messageMethod = MessageMethod::FixedUpdate;
        }
        else
        {
            return false;
        }
        return true; 
    }

};