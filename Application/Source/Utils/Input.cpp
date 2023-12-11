#include "Input.h"
#include "Core/Window.h"
#include "Core/Application.h"
#include "GLFW//glfw3.h"
#include <iostream>

glm::vec2 Input::GetMouseOffset()
{
	if (Application::Instance()->GetWindow().IsCursorEnabled())
	{
		return { 0, 0 };
	}

	static bool firstMouse = true;
	if (firstMouse)
	{
		lastMouseCoords_ = mouseCoords_;
		firstMouse = false;
	}

	auto offset = mouseCoords_ - lastMouseCoords_;
	//_lastMouseCoords = _mouseCoords;

	return offset;
}

bool Input::GetMouseButtonDown(int button)
{
	const auto& action = _buttonActionMap.find(button);
	return action != _buttonActionMap.end() ? action->second == GLFW_PRESS : false;
}

bool Input::GetMouseButtonUp(int button)
{
	const auto& action = _buttonActionMap.find(button);
	return action != _buttonActionMap.end() ? action->second == GLFW_RELEASE : false;
}

void Input::UpdateMouseCoords(glm::vec2 coords)
{
	mouseCoords_ = coords;
}

void Input::UpdateMouseButtonMap(int button, int action)
{
	//if (_buttonActionMap.find(button) != _buttonActionMap.end())
	_buttonActionMap[button] = action;
}

glm::vec2 Input::GetMouseCoords() const
{
	return mouseCoords_;
}

void Input::Update()
{
	lastMouseCoords_ = mouseCoords_;
	_buttonActionMap.clear();
}

bool Input::IsKeyDown(const int key)
{
	// bitset::test returns true if the bit at that position is 1. Otherwise return false
	return currStatus_.test(key);
}

bool Input::IsKeyUp(const int key)
{
	// Will now return true if the bit at that position is 0, otherwise return false
	return !currStatus_.test(key);
}

bool Input::IsKeyPressed(const int key)
{
	// Compare between prevStatus_ and currStatus_
	// Check if prevStatus_[key] is 0 and currStatus_[key] is 1
	return !prevStatus_.test(key) && IsKeyDown(key);
}

bool Input::IsKeyReleased(const int key)
{
	// Compare between prevStatus_ and currStatus_
	// Check if prevStatus_[key] is 1 and currStatus_[key] is 0
	return prevStatus_.test(key) && IsKeyUp(key);
}

void Input::UpdateKeys(const int key, const int action)
{
	// Check if OOB
	if (key >= 0 && key < MAX_KEYS)
	{
		// Update curr status so we update prevstatus accordingly
		prevStatus_.set(key, currStatus_[key]);
		// Update curr status
		currStatus_.set(key, action);
	}
}

void Input::PostUpdateKeys(void)
{
	// Update all keys (post-update)
	// Prep for next event
	prevStatus_ = currStatus_;
}

void Input::ResetAllKeys(void)
{
	// Iterate through all the elements in the bitset and set to 0 (false), representing key up
	for (int i = 0; i < MAX_KEYS; i++)
	{
		currStatus_[i] = false;
		prevStatus_[i] = false;
	}
}

void Input::ResetKey(const int key)
{
	currStatus_[key] = false;
	prevStatus_[key] = false;
}

glm::vec2 Input::GetScrollOffset(void)
{
	return mouseScrollOffset_;
}

void Input::UpdateScrollOffset(const glm::vec2& offset)
{
	mouseScrollOffset_ = offset;
}