#pragma once
#include <vector>
#include "Core/Layer.h"
#include "GameObject.h"

class Camera;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

	inline virtual std::shared_ptr<GameObject> CreateGameObject()
	{
		std::shared_ptr<GameObject> temp = std::make_shared<GameObject>();
		objects_.push_back(temp);
		return temp;
	}

	// For loading
	int applicationRefreshes_;

protected:
	Camera* sceneCamera_;
	std::vector<std::shared_ptr<GameObject>> objects_;
	friend class Application;

	bool isChangingScene_ = false;
	std::string sceneNameToChangeTo_;

};

