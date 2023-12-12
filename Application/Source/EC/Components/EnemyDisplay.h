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

    void Init(Ref<Tilemap> tilemap, Ref<EnemyPool> minerPool, Ref<EnemyPool> knightPool, 
        Ref<EnemyPool> witchPool, Ref<EnemyPool> banditPool);
    void AddMinerToPool(Ref<EnemyController> miner);
    void AddKnightToPool(Ref<EnemyController> knight);
    void AddWitchToPool(Ref<EnemyController> witch);
    void AddBanditToPool(Ref<EnemyController> bandit);

    inline void SetMinerBounds(glm::ivec2 min, glm::ivec2 max)
    {
        minerBoundsMin_ = min;
        minerBoundsMax_ = max;
    }

    inline void SetKnightBounds(glm::ivec2 min, glm::ivec2 max)
    {
        knightBoundsMin_ = min;
        knightBoundsMax_ = max;
    }

    inline void SetWitchBounds(glm::ivec2 min, glm::ivec2 max)
    {
        witchBoundsMin_ = min;
        witchBoundsMax_ = max;
    }

    inline void SetBanditBounds(glm::ivec2 min, glm::ivec2 max)
    {
        banditBoundsMin_ = min;
        banditBoundsMax_ = max;
    }

    bool InBounds(glm::ivec2 coords);

private:
    void TextCentred(std::string text);
    void TextCentred(float value);

    void UpdateText();

    Ref<EnemyController> lastClickedEnemy_;

    Ref<EnemyPool> chosenPool_;
    Ref<EnemyPool> minerPool_, knightPool_, witchPool_, banditPool_;
    glm::ivec2 minerBoundsMin_, minerBoundsMax_, knightBoundsMin_, knightBoundsMax_,
        witchBoundsMin_, witchBoundsMax_, banditBoundsMin_, banditBoundsMax_;
    glm::ivec2 min_, max_;
    Ref<Tilemap> tilemap_;

    float timer_ = 0.0f;
    int currentSelectedTeam_ = 1;

    std::string enemyName_;
    std::string enemyTeam_;
    std::string enemyCurrentHealth_;
    std::string enemyCurrentState_;
};