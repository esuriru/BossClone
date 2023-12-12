#pragma once

#include "EC/Component.h"

#include "EC/Components/Tilemap.h"
#include "Game/EnemyPool.h"

class EnemyController;

class EnemyDisplay : public Component
{
public:
    EnemyDisplay(GameObject& gameObject);

    void Update(Timestep ts) override;
    void OnImGuiRender() override;

    void Init(Ref<Tilemap> tilemap, Ref<EnemyPool> minerPool, Ref<EnemyPool> knightPool);
    void AddMinerToPool(Ref<EnemyController> miner);
    void AddKnightToPool(Ref<EnemyController> knight);

private:
    void TextCentred(std::string text);

    Ref<EnemyController> lastClickedEnemy_;

    Ref<EnemyPool> minerPool_, knightPool_;
    Ref<Tilemap> tilemap_;

    std::string enemyName_;
    std::string enemyCurrentHealth_;
    std::string enemyCurrentState_;
};