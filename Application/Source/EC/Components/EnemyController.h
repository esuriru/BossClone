#pragma once

#include <utility>

#include "EC/Component.h"

#include "Game/StateMachine.h"

#include "EC/Components/Tilemap.h"
#include "EC/Components/MineController.h"
#include "EC/Components/Pathfinder.h"
#include "EC/Components/TilemapEntity.h"
#include "EC/Components/Pathfinder.h"

#include "Game/EightWayDirectionFlags.h"

#include <glm/glm.hpp>
#include "Utils/DFS.h"


class BoxCollider2D;
class EnemyPool;

class EnemyController : public TilemapEntity, public std::enable_shared_from_this<EnemyController>
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;
    void FixedUpdate(float fixedDeltaTime) override;

    virtual void Reset();

    inline EnemyController* SetTilemap(Ref<Tilemap> tilemap) override
    {
        tilemap_ = tilemap;
        if (!dfs_)
        {
            dfs_ = CreateScope<DFS>(tilemap);
        }
        return this;
    }

    inline EnemyController* SetVisibilityTilemap(Ref<Tilemap> tilemap) override
    {
        visibilityTilemap_ = tilemap;
        return this;
    }

    inline EnemyController* SetVisibilityRange(uint8_t range) override
    {
        visibilityRange_ = range;
        return this;
    }
    
    inline EnemyController* SetDamage(float damage)
    {
        damage_ = damage;
        return this;
    }

private:
    void MoveWithDFS();
    void MoveInRandomAvailableDirection();
    void Attack();

    Ref<Pathfinder> pathfinder_ = nullptr;
    float damage_ = 0.0f;
    int turnsInChase_ = 0;

protected:
    Ref<TilemapEntity> entityChasing_ = nullptr;
    Scope<StateMachine<>> stateMachine_;
    Scope<DFS> dfs_ = nullptr;

    void OnDeath() override;
};