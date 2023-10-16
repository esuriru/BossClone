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
    Transition(T fromID, T toID)
        : fromID_(fromID)
        , toID_(toID)
    {

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
        callback_();
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