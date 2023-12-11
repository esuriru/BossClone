#include "KnightController.h"

#include "EC/Components/Collider2D.h"
#include "Physics/PhysicsWorld.h"
#include "MinerController.h"

KnightController::KnightController(GameObject &gameObject)
    : EnemyController(gameObject)
    , targetCollider_(nullptr)
    , minerCollider_(nullptr)
{
    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Idle State"),
            ActionEntry("FixedUpdate", 
            [&]()
            {
                Scan();
            }) 
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Patrol State"),
            ActionEntry("Enter", 
            [&]()
            {
                timer_ = 0.0f;
                localTilemapPosition_ = 
                    tilemap_->WorldToLocal(GetTransform().GetPosition());
                GenerateNewLocation(GetPossibleDirections());
            }),
            ActionEntry("Update", 
            [&]()
            {
                Move();
            }) 
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Guard State"),
            ActionEntry("Enter", 
            [&]()
            {
                timer_ = 0.0f;
                localTilemapPosition_ = 
                    tilemap_->WorldToLocal(GetTransform().GetPosition());
                
                auto position = tilemap_->WorldToLocal(
                    minerCollider_->GetTransform().GetPosition());
                targetTilemapPosition_ = position + 
                    Utility::ConvertDirection(
                    GetRandomDirection(
                    GetPossibleDirections(
                        position
                    )));
                GenerateNewLocation(GetNextTargetDirections());
            }),
            ActionEntry("Update", 
            [&]()
            {
                Move();
                if (timer_ >= 1.0f)
                {
                    timer_ = 0.0f;
                    Scan();
                    localTilemapPosition_ = 
                        tilemap_->WorldToLocal(GetTransform().GetPosition());
                    auto position = tilemap_->WorldToLocal(
                        minerCollider_->GetTransform().GetPosition());
                    if (localTilemapPosition_ == position)
                    {
                        // Stop randomising
                        timer_ = 0.0f;
                        return;
                    }

                    targetTilemapPosition_ = position + 
                        Utility::ConvertDirection(
                        GetRandomDirection(
                        GetPossibleDirections(
                            position
                        )));
                    GenerateNewLocation(GetNextTargetDirections());
                }
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Chase State"),
            ActionEntry("Enter", 
            [&]()
            {
                // Instantly check
                // if (glm::distance(targetCollider_->GetTransform().GetPosition(), 
                //     GetTransform().GetPosition()) <= attackDistance_)
                // {
                //     // Transition to attack if we are close enough
                //     transitionAttack_ = true;
                //     return;
                // }
                // else
                // {
                    timer_ = 0.0f;
                    targetTilemapPosition_ = tilemap_->WorldToLocal(
                        targetCollider_->GetTransform().GetPosition());
                // }
            }),
            ActionEntry("Update", 
            [&]()
            {
                Move();
            }) 
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Attack State"),
            ActionEntry("Enter", 
            [&]()
            {
                CC_TRACE("Attack");
            }),
            ActionEntry("Exit", 
            [&]()
            {
                targetCollider_->GetGameObject().Message("Run");
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Cooldown State"),
            ActionEntry("Enter", 
            [&]()
            {
                cooldownTimer_ = 0.0f;
            }),
            ActionEntry("Update", 
            [&]()
            {
                cooldownTimer_ += stateMachine_->currentTimestep;
            })
        )
    );
    
    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Cooldown State"),
            std::string("Chase State"),
            [&]()
            {
                if (cooldownTimer_ >= cooldownTime_)
                {
                    if (targetCollider_ && 
                        targetCollider_->GetGameObject().ActiveSelf() &&
                        glm::distance(targetCollider_->GetTransform().GetPosition(), 
                            GetTransform().GetPosition()) <= maxFollowDistance_ && 
                        minerCollider_ &&
                        glm::distance(minerCollider_->GetTransform().GetPosition(), 
                            GetTransform().GetPosition()) <= tetherDistance_)
                    {
                        return true;
                    }
                    transitionBack_ = true;
                    return false;
                }
                return false;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                transitionBack_ = false;
            },
            std::string("Cooldown State"),
            std::string("Guard State"),
            [&]()
            {
                return transitionBack_;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Guard State"),
            std::string("Chase State"),
            [&]()
            {
                return targetCollider_ && 
                    targetCollider_->GetGameObject().ActiveSelf() &&
                    glm::distance(targetCollider_->GetTransform().GetPosition(), 
                        GetTransform().GetPosition()) <= maxFollowDistance_;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                transitionAttack_ = false;
            },
            std::string("Chase State"),
            std::string("Attack State"),
            [&]()
            {
                if (transitionAttack_)
                {
                    return true;
                }
                if (timer_ >= 1.0f)
                {
                    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
                    targetTilemapPosition_ = tilemap_->WorldToLocal(
                        targetCollider_->GetTransform().GetPosition());
                    if (minerCollider_ && (glm::distance(minerCollider_->GetTransform().GetPosition(),
                        GetTransform().GetPosition()) > tetherDistance_))
                    {
                        if (!minerCollider_->GetGameObject()
                            .ActiveSelf())
                        {
                            minerCollider_ = nullptr;
                            timer_ = 0.0f;
                        }
                        transitionBack_ = true;
                        return false;
                    }

                    if (localTilemapPosition_ == targetTilemapPosition_)
                    {
                        // Transition to attack if we are close enough
                        return true;
                    }

                    GenerateNewLocation(GetNextTargetDirections());
                    timer_ = 0.0f;
                }
                return false;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                transitionBack_ = false;
            },
            std::string("Chase State"),
            std::string("Guard State"),
            [&]()
            {
                return transitionBack_ && minerCollider_;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                transitionBack_ = false;
            },
            std::string("Chase State"),
            std::string("Idle State"),
            [&]()
            {
                return transitionBack_ && !minerCollider_;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Patrol State"),
            std::string("Idle State"),
            [&]()
            {
                if (timer_ >= 1.0f)
                {
                    Scan();
                    if (targetCollider_ == nullptr)
                    {
                        return true;
                    }
                    timer_ = 0.0f;
                } 
                return false;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Idle State"),
            std::string("Guard State"),
            [&]()
            {
                return static_cast<bool>(minerCollider_);
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Patrol State"),
            std::string("Guard State"),
            [&]()
            {
                return static_cast<bool>(minerCollider_);
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Patrol State"),
            std::string("Chase State"),
            [&]()
            {
                return static_cast<bool>(minerCollider_) && 
                    static_cast<bool>(targetCollider_);
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Guard State"),
            std::string("Idle State"),
            [&]()
            {
                if (minerCollider_ && !minerCollider_->GetGameObject().ActiveSelf())
                {
                    // Object has died
                    minerCollider_ = nullptr;
                    return true;
                }
                return false;
            } 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<TimedTransition<>>(
            std::string("Attack State"),
            std::string("Cooldown State"),
            0.5f
        )
    );

    stateMachine_->AddTransition(
        CreateScope<TimedTransition<>>(
            std::string("Idle State"),
            std::string("Patrol State"),
            1.0f
        )
    );

    stateMachine_->InitateStartState("Idle State");
}

void KnightController::OnTriggerEnter2D(Collider2D *other)
{

}

void KnightController::Move()
{
    timer_ += stateMachine_->currentTimestep;

    GetTransform().SetPosition(
        Utility::Lerp(GetTransform().GetPosition(),
        currentTargetPosition_,
        timer_));
    if (arrowObject_)
    {
        arrowObject_->GetTransform().SetPosition(
            GetTransform().GetPosition());
    }

    // Scan();

}

void KnightController::Reset()
{
    EnemyController::Reset();
    minerCollider_ = nullptr;
    targetCollider_ = nullptr;
}


void KnightController::Scan()
{
    auto colliders = 
        PhysicsWorld::Instance()->CircleCast(GetTransform().GetPosition(), scanRadius_, false); 
    
    for (auto collider : colliders)
    {
        if (collider->GetGameObject().CompareTag("Miner") && !minerCollider_ &&
            !collider->GetGameObject().GetComponent<MinerController>()->IsGuarded())
        {
            minerCollider_ = collider;
            collider->GetGameObject().Message("Guard");
        }
        if (collider->GetGameObject().CompareTag("Witch") && !targetCollider_)
        {
            targetCollider_ = collider;
        }
    }

}

void KnightController::GenerateNewLocation()
{
    auto randomDirection = GetRandomDirection(GetPossibleDirections());
    if (randomDirection == EightWayDirection::None)
    {
        CC_ERROR("Direction None");
        return;
    }
    currentTargetPosition_ = tilemap_->LocalToWorld(localTilemapPosition_
        + Utility::ConvertDirection(randomDirection));
}

void KnightController::GenerateNewLocation(EightWayDirectionFlags flags)
{
    auto randomDirection = GetRandomDirection(flags);
    if (randomDirection == EightWayDirection::None)
    {
        CC_ERROR("Direction None");
        return;
    }
    currentTargetPosition_ = tilemap_->LocalToWorld(localTilemapPosition_
        + Utility::ConvertDirection(randomDirection));
}

