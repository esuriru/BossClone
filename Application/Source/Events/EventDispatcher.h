#pragma once

#include "Event.h"
#include <functional>

class EventDispatcher
{
	template<typename T>
	using EventHandler = std::function<bool(T&)>;
public:
	EventDispatcher(Event& event) : event_(event) {}

	template<typename T>
	bool Dispatch(EventHandler<T> callback)
	{
		if (event_.GetEventType() == T::GetStaticType())
		{
			// Use the handler argument to handle the event.
			event_.Handled = callback(*(T*)&event_);
			return true;
		}
		return false;
	}

private:
	Event& event_;

};
	

