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

    void Init(Ref<Tilemap> tilemap, Ref<EnemyPool> minerPool);
    void AddMinerToPool(Ref<EnemyController> miner);

private:
    void TextCentred(std::string text);

    Ref<EnemyController> lastClickedEnemy_;

    Ref<EnemyPool> minerPool_;
    Ref<Tilemap> tilemap_;

    std::string enemyName_;
    std::string enemyCurrentHealth_;
    std::string enemyCurrentState_;
};