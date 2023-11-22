#pragma once

#include <string>

#include "Transition.h"

template<typename T = std::string>
class TimedTransition : public Transition<T>
{
private:
    float initialSeconds_;
    float timer_;

public:
    TimedTransition(T fromID, T toID, float seconds)
        : Transition<T>(fromID, toID)
        , initialSeconds_(seconds)
        , timer_(seconds)
    {

    }

    void ResetTimer()
    {
        timer_ = initialSeconds_;
    }

    void Update(Timestep ts) override
    {
        timer_ -= ts;
    }

    bool TestConditions() override 
    {
        return timer_ <= 0.0f;
    }
};