#pragma once

#include "EnemyController.h"

class BanditController : public EnemyController
{
public:
    BanditController(GameObject& gameObject);

    void Message(std::string message) override;

    void Reset() override;

protected:
    float scanRadius_ = 15.0f;
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