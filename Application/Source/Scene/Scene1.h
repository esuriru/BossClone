#ifndef SCENE_1_H
#define SCENE_1_H

#include <deque>
#include "GameObject.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/Texture.h"
#include <glm/glm.hpp>
#include "Scene.h"
#include "Renderer/Mesh.h"
#include "MatrixStack.h"
#include "Components/Camera.h"
#include "Components/Droppable.h"

#include "Components/FinishPoint.h"

class Scene1 : public Scene
{
private:
	FinishPoint* _fp;
public:
	Scene1();
	~Scene1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	virtual GameObject* CreateGameObject();

private:
	void HandleKeyPress();

	ShaderLibrary* shaderLib_;
	//void RenderMesh(Mesh* mesh, bool enableLight);

	std::vector<GameObject*> gameObjects_;

	std::vector<Droppable*> drops_;
	std::deque<Droppable*> dropQueue_;
	std::vector<Mesh*> toCleanUp_;
	std::unordered_map<Droppable*, bool> currentlyDropping_;
	Droppable* currentDrop_;

	float worldWidth_m;
	float worldHeight_;
	double timePassed_ = 0;

	Material* yellowMat_;
	Material* playerMat_;
};

#endif