#include "WitchController.h"

#include "EC/Components/Collider2D.h"
#include "Physics/PhysicsWorld.h"

WitchController::WitchController(GameObject &gameObject)
    : EnemyController(gameObject)
    , targetCollider_(nullptr)
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
                targetCollider_ = nullptr;
                timer_ = 0.0f;
                localTilemapPosition_ = 
                    tilemap_->WorldToLocal(GetTransform().GetPosition());
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
            std::string("Chase State"),
            ActionEntry("Enter", 
            [&]()
            {
                // Instantly check
                if (glm::distance(targetCollider_->GetTransform().GetPosition(), 
                    GetTransform().GetPosition()) <= attackDistance_)
                {
                    // Transition to attack if we are close enough
                    transitionAttack_ = true;
                    return;
                }
                else
                {
                    timer_ = 0.0f;
                    targetTilemapPosition_ = tilemap_->WorldToLocal(
                        targetCollider_->GetTransform().GetPosition());
                }
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
                            GetTransform().GetPosition()) <= maxFollowDistance_)
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
                targetCollider_ = nullptr;
            },
            std::string("Cooldown State"),
            std::string("Patrol State"),
            [&]()
            {
                return transitionBack_;
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
                    if (glm::distance(targetCollider_->GetTransform().GetPosition(), 
                        GetTransform().GetPosition()) <= attackDistance_)
                    {
                        // Transition to attack if we are close enough
                        return true;
                    }
                    targetTilemapPosition_ = tilemap_->WorldToLocal(
                        targetCollider_->GetTransform().GetPosition());

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
                targetCollider_ = nullptr;
            },
            std::string("Chase State"),
            std::string("Patrol State"),
            [&]()
            {
                return glm::distance(targetCollider_->GetTransform().GetPosition(), 
                    GetTransform().GetPosition()) > maxFollowDistance_ ||
                    !targetCollider_->GetGameObject().ActiveSelf();
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
                    if (targetCollider_ != nullptr)
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
            std::string("Patrol State"),
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

void WitchController::OnTriggerEnter2D(Collider2D *other)
{
    // if (other->GetGameObject().CompareTag("Miner"))
    // {
    //     targetCollider_ = other;
    // }
}

void WitchController::Move()
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

}

void WitchController::GenerateNewLocation()
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

void WitchController::GenerateNewLocation(EightWayDirectionFlags flags)
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

void WitchController::Message(std::string message)
{
    if (message == "Run")
    {
        currentHealth_ -= 20.0f;
        if (currentHealth_ <= 0.0f)
        {
            gameObject_.SetActive(false);
        }
    }
}

void WitchController::Scan()
{
    auto colliders = 
        PhysicsWorld::Instance()->CircleCast(GetTransform().GetPosition(), scanRadius_, false); 
    
    for (auto collider : colliders)
    {
        if (collider->GetGameObject().CompareTag("Miner") && !targetCollider_)
        {
            targetCollider_ = collider;
        }
    }
}
