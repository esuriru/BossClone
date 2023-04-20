#pragma once

#include "Event.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int w, unsigned int h) :  width_(w), height_(h) {}

	inline unsigned int GetWidth() const
	{
		return width_;
	}

	inline unsigned int GetHeight() const
	{
		return height_;
	}

	inline glm::vec2 GetSize() const
	{
		return { width_, height_ };
	}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << width_ << " x " << height_;
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(WindowResize)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
	unsigned int width_, height_;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	NR_EVENT_CLASS_TYPE(WindowClose)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppTickEvent : public Event
{
public:
	AppTickEvent() = default;

	NR_EVENT_CLASS_TYPE(AppTick)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() = default;

	NR_EVENT_CLASS_TYPE(AppUpdate)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppRenderEvent : public Event
{
public:
	AppRenderEvent() = default;

	NR_EVENT_CLASS_TYPE(AppRender)
	NR_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};


