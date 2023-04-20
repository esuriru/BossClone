#include "SceneManager.h"
#include "Core/Core.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	scenes_.clear();
}

std::shared_ptr<Scene> SceneManager::GetScene(const std::string& name) const
{
	if (scenes_.find(name) == scenes_.end())
	{
		CC_WARN("Scene could not be found. Name: ", name);
		return nullptr;
	}
	return scenes_.at(name);
}


