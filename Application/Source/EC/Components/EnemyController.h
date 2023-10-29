#pragma once

#include <utility>

#include "EC/Component.h"

#include "Game/StateMachine.h"

#include "EC/Components/Tilemap.h"
#include "EC/Components/MineController.h"
#include "EC/Components/Pathfinder.h"

class EnemyController : public Component
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;

    void OnTriggerEnter2D(Collider2D* other) override;
    
    EnemyController* SetTilemap(Ref<Tilemap> tilemap);
    EnemyController* SetMineController(Ref<MineController> mineController);
private:
    Scope<StateMachine<>> stateMachine_;

    Ref<Tilemap> tilemap_;
    Ref<MineController> mineController_;

    std::pair<uint32_t, uint32_t> localTilemapPosition_;
};