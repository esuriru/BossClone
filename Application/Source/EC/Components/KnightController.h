#pragma once

#include "EnemyController.h"

class KnightController : public EnemyController
{
public:
    KnightController(GameObject& gameObject);

    void OnTriggerEnter2D(Collider2D* other) override;

    void Move() override;

    inline void SetTetherDistance(float tetherDistance)
    {
        tetherDistance_ = tetherDistance;
    }

    inline void SetMaxFollowDistance(float maxFollowDistance)
    {
        maxFollowDistance_ = maxFollowDistance;
    }

    void Reset() override;

protected:
    float scanRadius_ = 20.0f;
    float tetherDistance_ = 30.0f;
    float maxFollowDistance_ = 20.0f;
    float cooldownTime_ = 2.0f;
    float cooldownTimer_;

    bool transitionBack_ = false;
    bool transitionAttack_ = false;

    Collider2D* targetCollider_;
    Collider2D* minerCollider_;

    void Scan();
    void GenerateNewLocation() override;
    void GenerateNewLocation(EightWayDirectionFlags flags);

    // EightWayDirectionFlags GetPossibleDirections() override;
};