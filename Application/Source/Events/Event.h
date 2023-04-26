#pragma once

#include <string>

// NOTE: Used by other event classes for their ToString implementation
#include <sstream>

#include "Core/Core.h"


enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, // Window events
	AppTick, AppUpdate, AppRender, // App events
	KeyPressed, KeyReleased, // Key events
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled, // Mouse events
};

/**
 * \brief Event categories are bit-shifted to create a bitfield which enables events to have
 * more than one category.
 */
enum EventCategory
{
	EventNone						= 0,
	EventCategoryApplication	= BIT(0),
	EventCategoryInput			= BIT(1),
	EventCategoryKeyboard		= BIT(2),
	EventCategoryMouse			= BIT(3),
	EventCategoryMouseButton	= BIT(4),
};

/**
* \brief Define the class type. (To be defined in every event)
* \param type: Event class type
*/
#define NR_EVENT_CLASS_TYPE(type)\
static EventType GetStaticType() { return EventType::##type; }\
virtual EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return #type; }

/**
* \brief Define the class category. (To be defined in an event that has a category)
* \param category: Event class category
*/
#define NR_EVENT_CLASS_CATEGORY(category)\
virtual int GetCategoryFlags() const override { return category; }

/**
 * \brief Internal events which are blocking. When an event occurs, it immediately gets dispatched
 * and must be dealt with immediately. In the future, a buffer might be put in place to process events
 * during the event part of the update stage.
 */
class Event
{
	friend class EventDispatcher;
public:
	// Does not need a constructor because this class itself will never be constructed. It is an interface.
	virtual ~Event() = default;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;

	inline virtual std::string ToString() const
	{
		return GetName();
	}

	inline bool IsInCategory(const EventCategory category) const
	{
		return GetCategoryFlags() & category;
	}

	inline friend std::ostream& operator<<(std::ostream& out, const Event& e)
	{
		return out << e.ToString();
	}
	bool Handled = false;
};


