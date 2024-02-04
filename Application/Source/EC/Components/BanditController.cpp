BanditController::BanditController(GameObject &gameObject)
    : EnemyController(gameObject)
    , targetCollider_(nullptr)
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
            std::string("Shoot State"),
            ActionEntry("Enter",
            [&]()
            {
                if (arrowObject_)
                {
                    arrowObject_->SetActive(true);
                    arrowObject_->GetTransform().SetPosition(GetTransform().GetPosition());
                }
            }),
            ActionEntry("Update", 
            [&]()
            {
                auto direction = targetCollider_->GetTransform().GetPosition() -  
                    GetTransform().GetPosition();
                if (glm::length(direction) > 0.05f)
                {
                    arrowObject_->GetTransform().SetRotation(glm::quat(
                        glm::vec3(0, 0, std::atan2(direction.y, direction.x))
                    ));
                }
            }),
            ActionEntry("Exit", 
            [&]()
            {
                targetCollider_->GetGameObject().Message("Bandit Shoot");
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Reload State"),
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
            std::string("Idle State"),
            std::string("Shoot State"),
            [&]()
            {
                return static_cast<bool>(targetCollider_);
            }
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Reload State"),
            std::string("Shoot State"),
            [&]()
            {
                if (cooldownTimer_ >= cooldownTime_)
                {
                    Scan();
                    return static_cast<bool>(targetCollider_);
                }
                return false;
            }
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Reload State"),
            std::string("Idle State"),
            [&]()
            {
                return !static_cast<bool>(targetCollider_);
            }
        )
    );

    stateMachine_->AddTransition(
        CreateScope<TimedTransition<>>(
            std::string("Shoot State"),
            std::string("Reload State"),
            0.75f
        )
    );

    stateMachine_->InitateStartState("Idle State");
}

void BanditController::GenerateNewLocation()
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

void BanditController::GenerateNewLocation(EightWayDirectionFlags flags)
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

void BanditController::Message(std::string message)
{
    if (message == "Run")
    {
        currentHealth_ -= 20.0f;
        if (currentHealth_ <= 0.0f)
        {
            OnDeath();
            if (team_ == 1)
            {
                GameManager::Instance()->SetTeamOneBandits(
                    GameManager::Instance()->GetTeamOneBandits() - 1
                );
            }
            else
            {
                GameManager::Instance()->SetTeamTwoBandits(
                    GameManager::Instance()->GetTeamTwoBandits() - 1
                );
            }
        }
    }
}

void BanditController::Reset()
{
    EnemyController::Reset();
    targetCollider_ = nullptr; 
    timer_ = 0.0f;
    cooldownTimer_ = 0.0f;
}

void BanditController::Scan()
{
    auto colliders = 
        PhysicsWorld::Instance()->CircleCast(GetTransform().GetPosition(), scanRadius_, false); 

    static std::array<std::string, 3> tags 
    {
        {
            std::string("Knight"),
            std::string("Witch"),
            std::string("Miner"),
        }
    };
    
    for (auto& collider : colliders)
    {
        if (&collider->GetGameObject() == &gameObject_ ||
            gameObject_.GetLayer() == 
                collider->GetGameObject().GetLayer())
        {
            continue;
        }

        for (auto& tag : tags)
        {
            if (collider->GetGameObject().CompareTag(tag))
            {
                targetCollider_ = collider;
                return;
            }
        }
    }
    targetCollider_ = nullptr;
}
