#include "Scene/SceneManager.h"
#include "GameObject.h"
#include "Physics/ColliderManager.h"

#include "Components/Transform.h"
#include "Components/MeshFilter.h"
#include "Components/Component.h"
#include "Core/Application.h"
#include "Renderer/SceneRenderer.h"

std::shared_ptr<GameObject> GameObject::Create()
{
	static Application* instance = Application::GetInstance();
	return instance->CreateGameObject();
}

GameObject::GameObject()
	: isActive_(true)
	, mainRenderer_(nullptr)
{
	// Initialize the game object
	transform_ = AddComponent<Transform>();
}

GameObject::~GameObject()
{
	for (auto& c : components_)
		if (c)
		{
			delete c;
			c = nullptr;
		}

	components_.clear();
}

void GameObject::Destroy(GameObject* go)
{
	for (auto& i : go->components_)
	{
		delete i;
		i = nullptr;
	}
	go->components_.clear();
	go->isActive_ = false;
}

bool GameObject::ActiveSelf() const
{
	return isActive_;
}

void GameObject::SetActive(bool b)
{
	isActive_ = b;

	if (b)
		for (const auto& i : components_)
			if (i->IsActive())
				i->SetDirty(true);
}

void GameObject::SetRenderer(Renderer* r)
{
	mainRenderer_ = r;
}

Renderer* GameObject::GetRenderer() const
{
	return mainRenderer_;
}

void GameObject::Update(double dt)
{
	//FixedUpdate(dt);
	for (const auto& i : components_)
	{
		if (Application::GetInstance()->loading)
			return;
		if (i->IsActive())
		{
			i->Update(dt);
		}
	}
}

static ColliderManager* collider_manager = ColliderManager::GetInstance();
void GameObject::FixedUpdate(double dt)
{
	//if (_timePassed >= FIXED_UPDATE_FRAME_TIME)
	//{
		for (const auto& i : components_)
		{
			if (i->IsActive())
				i->FixedUpdate();
		}
	//	_timePassed -= FIXED_UPDATE_FRAME_TIME;
	//}
}

void GameObject::BeforeFixedUpdate(double dt)
{
	//if (_timePassed >= FIXED_UPDATE_FRAME_TIME)
	//{
		for (const auto& i : components_)
		{
			if (i->IsActive())
				i->UpdateGravity();
		}
	//}
}

void GameObject::Render()
{
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	if (mainRenderer_ != nullptr && mainRenderer_->IsActive())
		renderer->QueueRender(mainRenderer_);
}

Transform* GameObject::GetTransform(void) const
{
	return transform_;
}

