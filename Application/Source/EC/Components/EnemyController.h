#pragma once

#include <utility>

#include "EC/Component.h"

#include "Game/StateMachine.h"

#include "EC/Components/Tilemap.h"
#include "EC/Components/MineController.h"
#include "EC/Components/Pathfinder.h"

#include "Game/EightWayDirectionFlags.h"

#include <glm/glm.hpp>

class BoxCollider2D;
class EnemyPool;

class EnemyController : public Component, public std::enable_shared_from_this<EnemyController>
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;
    void FixedUpdate(float fixedDeltaTime) override;

    virtual void Reset();
    virtual void Init();

    virtual void Move();

    virtual void OnDeath();

    virtual EnemyController* SetTilemap(Ref<Tilemap> tilemap);

    inline void SetArrowObject(Ref<GameObject> arrow)
    {
        arrowObject_ = arrow;
    }

    virtual void SetBounds(glm::ivec2 min, glm::ivec2 max);

    inline float GetHealth()
    {
        return currentHealth_;
    }

    void SetStartingHealth(float health)
    {
        currentHealth_ = health;
        initialHealth_ = health;
    }

    void SetTeam(int team);

    int GetTeam()
    {
        return team_;
    }

    void SetPool(Ref<EnemyPool> pool);

    std::string GetCurrentStateName();
protected:
    Scope<StateMachine<>> stateMachine_;

    Ref<GameObject> arrowObject_ = nullptr;

    Ref<Tilemap> tilemap_;
    Ref<BoxCollider2D> collider_;

    Ref<EnemyPool> pool_;

    float timer_ = 0.0f;
    float initialHealth_;
    float currentHealth_ = 100.0f;

    bool isMoving_ = false;

    int team_ = 0;

    glm::ivec2 localTilemapPosition_;
    glm::ivec2 targetTilemapPosition_;
    glm::vec3 currentTargetPosition_;

    glm::ivec2 boundsMin_ = { 0, 0 }, 
        boundsMax_ = { Tilemap::MaxHorizontalLength, 
            Tilemap::MaxVerticalLength};

    virtual void GenerateNewLocation();

    bool InBounds(glm::ivec2 coords) const;
    bool InBounds(int x, int y) const;

    virtual EightWayDirection GetRandomDirection(EightWayDirectionFlags flags);
    virtual EightWayDirectionFlags GetPossibleDirections();
    virtual EightWayDirectionFlags GetPossibleDirections(glm::ivec2 position);
    virtual EightWayDirectionFlags GetNextTargetDirections();
     
};