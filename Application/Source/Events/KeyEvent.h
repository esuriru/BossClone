#pragma once

#include "Event.h"
#include <sstream>

class KeyEvent : public Event
{
public:
	inline int GetKeyCode() const
	{
		return keycode_;
	}

	NR_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	KeyEvent(int keycode) : keycode_(keycode) {}
	int keycode_;

};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keycode, int repeatCount)
		: KeyEvent(keycode), repeatCount_(repeatCount) {}

	inline int GetRepeatCount() const
	{
		return repeatCount_;
	}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: Keycode: " << keycode_ << " (" << repeatCount_ << " repeats)";
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(KeyPressed)
private:
	int repeatCount_;
};

class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

	inline std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: Keycode: " << keycode_;
		return ss.str();
	}

	NR_EVENT_CLASS_TYPE(KeyReleased)
};
