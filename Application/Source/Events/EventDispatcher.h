#pragma once

#include "Event.h"
#include <functional>

class EventDispatcher
{
public:
	EventDispatcher(Event& event) : event_(event) {}

	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (event_.GetEventType() == T::GetStaticType())
		{
			// Use the handler argument to handle the event.
			event_.Handled = func(static_cast<T&>(event_));
			return true;
		}
		return false;
	}

private:
	Event& event_;

};
	

