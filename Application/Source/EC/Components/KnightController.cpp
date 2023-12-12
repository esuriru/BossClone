#include "KnightController.h"

#include "EC/Components/Collider2D.h"
#include "Physics/PhysicsWorld.h"
#include "MinerController.h"

#include "Game/GameManager.h"

KnightController::KnightController(GameObject &gameObject)
    : EnemyController(gameObject)
    , targetCollider_(nullptr)
    , minerCollider_(nullptr)
{
    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Idle State"),
            ActionEntry("Enter", 
            [&]()
            {
                if (arrowObject_)
                {
                    arrowObject_->SetActive(false);
                }
            }),
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
                minerCollider_ = nullptr; 
                localTilemapPosition_ = 
                    tilemap_->WorldToLocal(GetTransform().GetPosition());
                if (arrowObject_)
                {
                    arrowObject_->SetActive(false);
                }
                GenerateNewLocation(GetPossibleDirections());
            }),
            ActionEntry("Update", 
            [&]()
            {
                if (timer_ < 1.0f)
                {
                    Move();
                }
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
                if (arrowObject_)
                {
                    arrowObject_->SetActive(true);
                }
                GenerateNewLocation(GetNextTargetDirections());
            }),
            ActionEntry("Update", 
            [&]()
            {
                Move();
                if (arrowObject_)
                {
                    auto direction = minerCollider_->GetTransform().GetPosition() -
                        GetTransform().GetPosition();
                    if (glm::length(direction) > 0.05f)
                    {
                        arrowObject_->GetTransform().SetRotation(glm::quat(
                            glm::vec3(0, 0, std::atan2(direction.y, direction.x))
                        ));
                    }
                }
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
                if (arrowObject_)
                {
                    arrowObject_->SetActive(true);
                }
                // }
            }),
            ActionEntry("Update", 
            [&]()
            {
                Move();
                if (arrowObject_)
                {
                    auto direction = targetCollider_->GetTransform().GetPosition() -
                        GetTransform().GetPosition();
                    if (glm::length(direction) > 0.05f)
                    {
                        arrowObject_->GetTransform().SetRotation(glm::quat(
                            glm::vec3(0, 0, std::atan2(direction.y, direction.x))
                        ));
                    }
                }
            }) 
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Attack State"),
            ActionEntry("Enter", 
            [&]()
            {
                if (arrowObject_)
                {
                    arrowObject_->SetActive(false);
                }
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
                    if (minerCollider_)
                    {
                        if (glm::distance(minerCollider_->GetTransform().GetPosition(),
                            GetTransform().GetPosition()) > tetherDistance_)
                        {
                            timer_ = 0.0f;
                            transitionBack_ = true;
                            return false;
                        }
                        if (!minerCollider_->GetGameObject()
                            .ActiveSelf())
                        {
                            minerCollider_ = nullptr;
                            timer_ = 0.0f;
                            transitionBack_ = true;
                            return false;
                        }

                        if (localTilemapPosition_ == targetTilemapPosition_)
                        {
                            // Transition to attack if we are close enough
                            return true;
                        }

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
                localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
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
                    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
                    Scan();
                    if (minerCollider_ != nullptr)
                    {
                        return false;
                    }
                    return true;
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

void KnightController::Message(std::string message)
{
    if (message == "Bandit Shoot")
    {
        currentHealth_ -= 10.0f;
        if (currentHealth_ <= 0.0f)
        {
            OnDeath();
            if (team_ == 1)
            {
                GameManager::Instance()->SetTeamOneKnights(
                    GameManager::Instance()->GetTeamOneKnights() - 1
                );
            }
            else
            {
                GameManager::Instance()->SetTeamTwoKnights(
                    GameManager::Instance()->GetTeamTwoKnights() - 1
                );
            }
        }
    }
}

void KnightController::Reset()
{
    EnemyController::Reset();
    cooldownTimer_ = 0.0f;
    timer_ = 0.0f;
    minerCollider_ = nullptr;
    targetCollider_ = nullptr;
}


void KnightController::Scan()
{
    auto colliders = 
        PhysicsWorld::Instance()->CircleCast(GetTransform().GetPosition(), scanRadius_, false); 
    
    for (auto collider : colliders)
    {
        if (collider->GetGameObject().GetLayer() == gameObject_.GetLayer())
        {
            if (collider->GetGameObject().CompareTag("Miner") && !minerCollider_ &&
                !collider->GetGameObject().GetComponent<MinerController>()->IsGuarded())
            {
                minerCollider_ = collider;
                collider->GetGameObject().Message("Guard");
            }
        }
        else if ((collider->GetGameObject().CompareTag("Witch") ||
            collider->GetGameObject().CompareTag("Bandit"))
            && !targetCollider_)
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

