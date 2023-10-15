#pragma once

#include "State.h"
#include "StateMessageMethod.h"

#include <string>
#include <functional>

using std::function;
using std::string;

class ActionEntry
{
private:
    string methodName_;
    function<void()> action_;

public:
    ActionEntry(string methodName, function<void()> action)
        : methodName_(methodName)
        , action_(action)
    {

    }

    inline string GetMethodName() const
    {
        return methodName_;
    }

    inline function<void()> GetAction() const
    {
        return action_;
    }

};