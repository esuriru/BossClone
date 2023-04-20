#include "Confetti.h"
#include "Core/Application.h"
#include "Scene/GameObject.h"
#include "Transform.h"

Confetti::Confetti(GameObject* go)
	: Component(go)
{
	go->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, 1.0f * Application::GetInstance()->GetWindowHeight(), 0 });
	go->GetTransform()->SetScale({ Application::GetInstance()->GetWindowWidth(), Application::GetInstance()->GetWindowHeight(), 1 });
}

void Confetti::Update(double dt)
{
	gameObject_->GetTransform()->Translate({0, dt * -200.f, 0});
	if (gameObject_->GetTransform()->GetPosition().y < Application::GetInstance()->GetWindowHeight() * -1.f)
		gameObject_->SetActive(false);
}