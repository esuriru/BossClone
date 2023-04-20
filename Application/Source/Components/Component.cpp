#include "Component.h"
#include "Scene/GameObject.h"


GameObject* Component::GetGameObject(void) const
{
	return gameObject_;
}

void Component::SetDirty(bool d)
{
	isDirty_ = d;
}

bool Component::IsDirty()
{
	return isDirty_;
}

void Component::SetActive(bool d)
{
	isActive_ = d;
}

bool Component::IsActive()
{
	return isActive_;
}

Component::Component(GameObject* entity)
	: gameObject_(entity)
	,isDirty_(false)
	,isActive_(true)	
{
}

void Component::FixedUpdate()
{
	// Do nothing lol.
}
