#pragma once

#include "EC/Component.h"
#include "Game/StateMachine.h"

class EnemyController : public Component
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
private:
    Scope<StateMachine<>> _stateMachine;
};