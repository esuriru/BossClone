#include "Interactable.h"
#include "Scene/GameObject.h"

Interactable::Interactable(GameObject* go)
	: Component(go)
	, timesUsable(1)
{
	go->tag = "Pickable";

}

