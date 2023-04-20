#pragma once
#include "Component.h"
#include "Renderer/Mesh.h"

class MeshFilter;
class RigidBody2D;
class Droppable : public Component
{
public:
	Droppable(GameObject* go);

	~Droppable() override;
	void Update(double dt);
	virtual void FixedUpdate() override;

	bool IsDropping() const;

protected:
	Material* mat_;
	RigidBody2D* rb_;
	MeshFilter* mf_;

	float lifetimeTimer_;
	float lifetime_;
	bool isDropped_;
};
