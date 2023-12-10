#pragma once

#include <vector>
#include <functional>
#include <string>

#include "Core/Timestep.h"

using std::vector;

template<typename T = std::string>
class Transition
{
private:
    using predicate = std::function<bool()>;

    vector<predicate> conditions_;
    function<void()> callback_;

    T fromID_;
    T toID_;

public:
    template <class... Args>
    Transition(T fromID, T toID, Args&& ... args)
        : fromID_(fromID)
        , toID_(toID)
    {
		using pack_expander = int[];
		static_cast<void>(pack_expander{ 0, 
            (static_cast<void>(
                AddCondition(std::forward<Args>(args))), 0)... });
    }

    template <class... Args>
    Transition(function<void()> callback, T fromID, T toID, Args&& ... args)
        : fromID_(fromID)
        , toID_(toID)
        , callback_(callback)
    {
		using pack_expander = int[];
		static_cast<void>(pack_expander{ 0, 
            (static_cast<void>(
                AddCondition(std::forward<Args>(args))), 0)... });
    }

    void AddCondition() {}
    void AddCondition(predicate condition)
    {
        conditions_.push_back(condition);
    }

    virtual void Update(Timestep ts) {}

    virtual T GetFromID()
    {
        return fromID_;
    }

    virtual T GetToID()
    {
        return toID_;
    }

    virtual void SetCallback(function<void()> callback)
    {
        callback_ = callback;
    }

    virtual void InvokeCallback()
    {
        if (callback_)
        {
            callback_();
        }
    }

    virtual bool TestConditions()
    {
        for (auto& condition : conditions_)
        {
            if (!condition())
            {
                return false;
            }
        }

        return true;
    }
};