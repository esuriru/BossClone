#pragma once

#include "EnemyController.h"

class WitchController : public EnemyController
{
public:
    WitchController(GameObject& gameObject);

    void OnTriggerEnter2D(Collider2D* other) override;

    void Move() override;

    inline void SetAttackDistance(float attackDistance)
    {
        attackDistance_ = attackDistance;
    }

    inline void SetMaxFollowDistance(float maxFollowDistance)
    {
        maxFollowDistance_ = maxFollowDistance;
    }

    void Message(std::string message) override;

    void Reset() override;

protected:
    float scanRadius_ = 25.0f;
    float attackDistance_ = 10.0f;
    float maxFollowDistance_ = 25.0f;
    float cooldownTime_ = 2.0f;
    float cooldownTimer_;

    bool transitionBack_ = false;
    bool transitionAttack_ = false;

    Collider2D* targetCollider_;
    void Scan();
    void GenerateNewLocation() override;
    void GenerateNewLocation(EightWayDirectionFlags flags);

    // EightWayDirectionFlags GetPossibleDirections() override;
};