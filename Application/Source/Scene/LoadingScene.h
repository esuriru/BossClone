#pragma once
#include "Scene.h"
#include <string>
#include "Renderer/Material.h"

class Font;
class Text;
class Mesh;

class LoadingScene : public Scene
{
public:
	LoadingScene();
	virtual ~LoadingScene() override;

	void ChangeScene();
	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;

	void BindScene(std::shared_ptr<Scene> scene);
	void UpdateText(std::string newMessage);

private:
	// For progress bar
	bool isLoading_;
	float inverseTimesLoad_;
	float counter_;

	std::shared_ptr<Scene> sceneToLoad_;
	Font* inconsolataFont_ = nullptr;
	Text* displayText_;
	Mesh* quadMesh_;
	Mesh* backgroundMesh_;
	Texture* backgroundTexture_;

	Material* progressBarMat_;
	Material* backgroundMat_;
	Transform* progressBarTransform_;
};

