#include "InteractButton.h"
#include "Scene/GameObject.h"

InteractButton::InteractButton(GameObject* go)
	: Component(go)
	, collider_(go->GetComponent<Collider>())
	, isInteracting_(false)
{
}

Interactable* InteractButton::Interact()
{
	isInteracting_ = true;
	isDirty_ = true;
	return interactable_;
}

void InteractButton::Bind(Interactable* i)
{
	interactable_ = i;
}

void InteractButton::Update(double dt)
{
	const auto& r = gameObject_->GetRenderer();
	if (r && isDirty_)
	{
		collider_ = gameObject_->GetComponent<Collider>();

		r->SetActive(!isInteracting_);
		isDirty_ = false;
	}
}

void InteractButton::Reset()
{
	if (collider_)
		collider_->SetActive(true);
	isInteracting_ = false;
	isDirty_ = true;
}
