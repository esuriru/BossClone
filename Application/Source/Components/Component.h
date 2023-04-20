#pragma once
#include <memory>

constexpr double FIXED_UPDATE_FRAME_TIME = 1 / 60.0;
// Not going to internally cache components.
// Forward declaration
class GameObject;

class Component
{
protected:
	GameObject* gameObject_;
	bool isDirty_;
	bool isActive_;
	//unsigned long _uid;
public:

	virtual ~Component() = default;

	GameObject* GetGameObject(void) const;

	void SetDirty(bool d);
	bool IsDirty();

	void SetActive(bool d);
	bool IsActive();
	
	explicit Component(GameObject* entity);
	Component(const Component& rhs) = delete;

	virtual void Update(double dt) = 0;
	virtual void FixedUpdate();

	virtual void UpdateGravity() {};

};

