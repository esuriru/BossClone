#pragma once
#include "Renderer/Texture.h"
#include "Component.h"
#include "Interactable.h"
#include "Text.h"
#include <vector>
#include <string>
class Talkable : public Interactable 
{
public:
	Talkable(GameObject* go);

	virtual void Use(UseData& data) override;
	void BindOutput(Text* text);

	void BindDialogue(const std::vector<std::string>& dialogue);


protected:
	std::vector<std::string> dialogue_;

	Text* outputText_;
};

