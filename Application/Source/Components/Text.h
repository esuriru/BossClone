#pragma once
#include "Transform.h"
#include "Renderer.h"
#include "Renderer/Mesh.h"
#include <string>
#include <glm/glm.hpp>

struct Font;

class Text : public Renderer 
{
public:
	Text(GameObject* go);
	virtual ~Text() override;

	virtual void Render() override;
	virtual void Update(double dt) override;

	void SetText(const std::string& text);
	const std::string& GetText() const;

	void EnableDialogueBox(bool enable);

	glm::vec3 colour;

	void Render(const glm::mat4& view, const glm::mat4& projection);

	// TODO - A canvas system, but overkill.
	bool onScreen = false;

	void BindDialogueBox(std::shared_ptr<GameObject> db, bool enable = true);


	Font* font;
private:
	bool enableDialogueBox_ = false;
	std::shared_ptr<GameObject> dialogueBox_;

	Shader* textShader_;
	Mesh* textMesh_;
	Transform* targetTransform_;

	//Texture* _fontTexture;
	std::string text_;
};

