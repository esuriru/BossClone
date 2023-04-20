#include "Talkable.h"
#include "Scene/GameObject.h"


Talkable::Talkable(GameObject* go)
	: Interactable(go)
{
	go->tag = "Talkable";
	timesUsable = 1;

	// Create the game object to display text.
}

void Talkable::Use(UseData& data)
{
	if (!outputText_)
		return;

	if (data.timesUsed == 1)
	{
		// Initialize.
		outputText_->GetGameObject()->SetActive(true);
		outputText_->EnableDialogueBox(true);
	}

	if (data.timesUsed <= timesUsable)
		outputText_->SetText(dialogue_[data.timesUsed - 1]);
	else
	{
		outputText_->GetGameObject()->SetActive(false);
		outputText_->EnableDialogueBox(false);
	}

}

void Talkable::BindOutput(Text* text)
{
	outputText_ = text;
	outputText_->GetGameObject()->SetActive(false);
}

void Talkable::BindDialogue(const std::vector<std::string>& dialogue)
{
	dialogue_ = dialogue;
	timesUsable = dialogue.size();
}
