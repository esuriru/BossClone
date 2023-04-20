#pragma once
#include "Singleton.h"
#include <unordered_map>
#include "Scene.h"
#include <memory>

class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

	template<typename T>
	std::shared_ptr<T> CreateScene(std::string name, bool init = false)
	{
		std::shared_ptr<T> scn = std::make_shared<T>();
		if (init)
			scn->Init();
		scenes_[name] = scn;
		return scn;
	}

	std::shared_ptr<Scene> GetScene(const std::string& name) const;
		
	bool IsChangingScene;
private:
	std::unordered_map<std::string, std::shared_ptr<Scene>>	scenes_;
	std::shared_ptr<Scene> loadingScene_;
};

