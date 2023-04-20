#pragma once
#include "Scene.h"
#include "Utils/Font.h"
#include <vector>
#include <glm/glm.hpp>

#include "Components/Skybox.h"
#include "Components/Camera.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/SceneRenderer.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene();
	virtual ~MainMenuScene() override;

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;

	void BindScene(std::shared_ptr<Scene> scene);

private:

	enum meshList
	{
		TITLE_MESH,
		NUM_MESH,
	};

	std::array<Mesh*, NUM_MESH> meshList_;

	std::vector<Material*> mats_;

	std::shared_ptr<Scene> sceneToLoad_;

	Skybox* skybox_;

	ShaderLibrary* shaderLib_;

	Font* inconsolataFont_ = nullptr;

	std::vector<LightData*> lights_{};

	Text* startMessage_;
};

