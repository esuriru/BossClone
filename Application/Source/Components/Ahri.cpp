#include "Ahri.h"
#include "Transform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Scene/GameObject.h"

Ahri::Ahri(GameObject* go)
	: Talkable(go)
	, state_(Ahri::AhriState::FIRST_TALK)
	, firstTalk_(true)
{
	dialogues_[FIRST_TALK] =
	{
		"Hey human!",
		"Can you find my precious toy for me?",
		"I will reward you with a hefty reward!"
	};

	dialogues_[TALK] =
	{
		"Hmm, you need a clue?",
		"Look around more! That's the spirit!"
	};

	dialogues_[FIND_YES] = 
	{
		"Oh thank you!",
		"Here you go!",
	};

	dialogues_[FIND_NO] = 
	{
		"No no no, this isn't it.",
		"Do try again!",
	};

	//_dialogues[FIND_YES]
	BindDialogue(dialogues_[state_]);
}

void Ahri::Use(UseData& data)
{
	static bool firstTalkBinded = false;
	static bool foundTeddy = false;

	if (firstTalk_)
	{
		BindDialogue(dialogues_[FIRST_TALK]);
		firstTalkBinded = true;
	}
	else
	{
		if (data.playerItemHolding)
		{
			if (data.playerItemHolding->label == "Teddy")
			{
				BindDialogue(dialogues_[FIND_YES]);
				foundTeddy = true;
			}
			else
				BindDialogue(dialogues_[FIND_NO]);
		}
		else
		{
			BindDialogue(dialogues_[TALK]);
		}
	}

	Talkable::Use(data);

	if (data.timesUsed > timesUsable)
	{
		if (firstTalk_ && firstTalkBinded)
		{
			firstTalkBinded = false;
			firstTalk_ = false;
		}
		else if (foundTeddy)
		{
			data.playerItemHolding->GetGameObject()->GetTransform()->SetParent(nullptr);
			GameObject::Destroy(data.playerItemHolding->GetGameObject());
			data.playerItemHolding = nullptr;
			foundTeddy = false;

			if (bindedObject_)
			{
				glm::vec3 spawn = glm::vec3(-20, 20, 10) + gameObject_->GetTransform()->GetPosition();

				bindedObject_->SetActive(true);
				bindedObject_->GetTransform()->SetPosition(spawn);
				const auto& col = bindedObject_->GetComponent<Collider>();
				if (col)
				{
					col->Initialize();
					const auto& rb = bindedObject_->GetComponent<RigidBody>();
					if (rb)
					{
						col->attachedRigidbody = rb;
						rb->Update(0);
					}
				}
			}

		}
	}
}

void Ahri::BindObject(std::shared_ptr<GameObject> obj)
{
	bindedObject_ = obj;
	if (obj)
		obj->SetActive(false);
}
