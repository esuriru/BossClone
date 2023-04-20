#pragma once
#include "Talkable.h"
#include <array>

class Ahri : public Talkable 
{
public:
	Ahri(GameObject* go);

	enum AhriState
	{
		NONE,
		FIRST_TALK,
		TALK,
		FIND_YES,
		FIND_NO,
		TOTAL_STATES,
	};

	void Use(UseData& data) override;

	void BindObject(std::shared_ptr<GameObject> obj);
private:
	AhriState state_;
	std::array<std::vector<std::string>, AhriState::TOTAL_STATES> dialogues_;
	bool firstTalk_;
	std::shared_ptr<GameObject> bindedObject_;

};

