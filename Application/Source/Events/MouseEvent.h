#pragma once

#include "Event.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(const glm::vec2& mouseCoords) : mouseCoordinates_(mouseCoords) {}

	inline glm::vec2 GetCoords() const
	{
		return mouseCoordinates_;
	}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << glm::to_string(mouseCoordinates_);
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(MouseMoved)
	NR_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
	glm::vec2 mouseCoordinates_;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(const glm::vec2& offset) : offset_(offset) {}

	inline glm::vec2 GetOffset() const
	{
		return offset_;
	}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << glm::to_string(offset_);
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(MouseScrolled)
	NR_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
	glm::vec2 offset_;
};

class MouseButtonEvent : public Event
{
public:
	inline int GetMouseButton() const
	{
		return button_;
	}

	// NOTE: In theory, this should never be called from this specific class.
	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << GetName() << ": " << button_;
		return ss.str();
	}

	NR_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
protected:
	MouseButtonEvent(const int button) : button_(button) {}
	int button_;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(const int button) : MouseButtonEvent(button) {}

	NR_EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(const int button) : MouseButtonEvent(button) {}

	NR_EVENT_CLASS_TYPE(MouseButtonReleased)
};



