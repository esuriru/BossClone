#pragma once

#include <vector>
#include <functional>

using std::vector;

template<typename T = string>
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
        conditions_.push_back(std::forward<Args>(args)...);
    }

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