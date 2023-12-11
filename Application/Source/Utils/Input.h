#pragma once
#include "Singleton.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <bitset>

class Input : public Utility::Singleton<Input>
{
public:
#pragma region MOUSE
	glm::vec2 GetMouseOffset();
	bool GetMouseButtonDown(int button);
	bool GetMouseButtonUp(int button);

	void UpdateMouseCoords(glm::vec2 coords);
	void UpdateMouseButtonMap(int button, int action);

	glm::vec2 GetMouseCoords() const;

#pragma endregion MOUSE

	void Update();

#pragma region KEYBOARD

	// Check if a key is down
	// \param key the identifier of the key to be queried
	// \returns True if the key is down, False if the key is up
	bool IsKeyDown(const int key);

	// Check if a key is up
	// \param key the identifier of the key to be queried
	// \returns True if the key is up, False if the key is down
	bool IsKeyUp(const int key);

	// Check if a key was pressed
	// \param key the identifier of the key to be queried
	// \returns True if the key was pressed, False if the key is not pressed
	bool IsKeyPressed(const int key);

	// Check if a key was released 
	// \param key the identifier of the key to be queried
	// \returns True if the key was released, False if the key is not released
	bool IsKeyReleased(const int key);

	// Perform update operation for a key
	void UpdateKeys(const int key, const int action);

	// Perform post-update operations
	void PostUpdateKeys(void);

	// Reset all keys
	void ResetAllKeys(void);

	// Reset a key
	void ResetKey(const int key);

#pragma endregion KEYBOARD

#pragma region SCROLL

	// Get scroll offset
	glm::vec2 GetScrollOffset(void);

	// Update scroll
	void UpdateScrollOffset(const glm::vec2& offset);

#pragma endregion SCROLL

private:

	// Number of keys
	const static int MAX_KEYS = 348;

	// Current mouse coordinates
	glm::vec2 mouseCoords_;

	// Previous mouse coordinates
	glm::vec2 lastMouseCoords_;

	// Scroll coordinates
	glm::vec2 mouseScrollOffset_;

	// Hashmap of the buttons to their actions
	std::unordered_map<int, int> _buttonActionMap;

	// Bitset for key statuses.
	// Each index represents an individual key.
	// 1 for pressed, 0 for not pressed (T/F)
	std::bitset<MAX_KEYS> currStatus_, prevStatus_;
};

