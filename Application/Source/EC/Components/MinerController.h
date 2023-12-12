#pragma once

#include <utility>

#include "EC/Component.h"

#include "Game/StateMachine.h"

#include "EC/Components/Tilemap.h"
#include "EC/Components/MineController.h"
#include "EC/Components/Pathfinder.h"
#include "EC/Components/EnemyController.h"

#include "Game/EightWayDirectionFlags.h"

#include <glm/glm.hpp>

class BoxCollider2D;

class MinerController : public EnemyController 
{
public:
    MinerController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;
    void Message(std::string message) override;

    void OnDeath() override;

    void OnTriggerEnter2D(Collider2D* other) override;
    MinerController* SetTilemap(Ref<Tilemap> tilemap) override;
    
    MinerController* SetMineController(Ref<MineController> mineController);

    void GenerateNewLocation() override;
    void Reset() override;

    inline bool IsGuarded()
    {
        return isGuarded_;
    }

    void SetRestPlace(const glm::ivec2& location);

private:
    Ref<MineController> mineController_;

    GameObject* currentChosenOre_;
    glm::ivec2 restPlace_ = glm::ivec2(0);

    bool isGuarded_;

    bool reachedOre_;
    bool runMessage_ = false;
    float scanTimer_ = 0.0f;
    float timeSpentMining_ = 0.0f;
};