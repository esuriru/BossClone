#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <typeinfo>
#include <string>
#include "Components/Renderer.h"
#include "Components/CircleCollider2D.h"


class Component;
class Transform;

class GameObject
{
protected:
	std::vector<Component*> components_;

	Transform* transform_; // Every game object has a transform.
	Renderer* mainRenderer_;

	double timePassed_;

	bool isActive_;

public:
	static std::shared_ptr<GameObject> Create();
	GameObject();
	~GameObject();

	static void Destroy(GameObject* go);

	std::string tag;

	bool ActiveSelf() const;
	void SetActive(bool b);
	void SetRenderer(Renderer* r);
	Renderer* GetRenderer() const;

	void Update(double dt);
	void FixedUpdate(double dt);
	void BeforeFixedUpdate(double dt);
	void Render();

	Transform* GetTransform(void) const;

	inline void Dirty()
	{
		for (auto& c : components_)
			c->SetDirty(true);
	}

#pragma region Inline_Functions
	template<typename T>
	inline T* AddComponent(void)
	{
		// There might be some components that a game object can store more than once (For example: Unity's WheelJoint), however, for this, we keep it to one.
		static_assert(std::is_base_of<Component, T>::value, "Invalid input parameter. Type must be a derived class of component.");
		if (GetComponent<T>() != nullptr) return nullptr;
		auto temp = CreateComponent<T>();
		components_.push_back(temp);
		for (auto& c : components_)
			c->SetDirty(true);
		return temp;
	}

	template<typename T>
	inline T* CreateComponent(void)
	{
		static_assert(std::is_base_of<Component, T>::value, "Invalid input parameter. Type must be a derived class of component.");
		return new T(this);
	}

	template<typename T>
	inline T* GetComponent(void)
	{
		static_assert(std::is_base_of<Component, T>::value, "Invalid input parameter. Type must be a derived class of component.");
		for (const auto& i : components_)
		{
			auto component = dynamic_cast<T*>(i);
			if (component)
				return component;
		}
		return nullptr;
	}
#pragma endregion Inline_Functions

};

