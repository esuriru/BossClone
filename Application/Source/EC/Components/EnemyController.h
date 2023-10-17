#pragma once

#include "EC/Component.h"

#include "Game/StateMachine.h"
#include "EC/Components/Tilemap.h"

class EnemyController : public Component
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;
    
    void SetTilemap(Ref<Tilemap> tilemap);
private:
    Scope<StateMachine<>> stateMachine_;
    Ref<Tilemap> tilemap_;

};