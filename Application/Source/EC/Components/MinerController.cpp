#include "MinerController.h"

#include "Utils/Input.h"

#include "Core/KeyCodes.h"
#include "Core/Core.h"

#include "EC/Components/Transform.h"
#include "EC/Components/Collider2D.h"
#include "EC/Components/BoxCollider2D.h"
#include "EC/GameObject.h"

#include "Game/StateMachine.h"
#include "Game/ActionEntry.h"

#include "Game/TimedTransition.h"

#include "Utils/Random.h"

#include "Game/EightWayDirectionFlags.h"
#include "Game/EnemyPool.h"

#include "Tilemap.h"

#include <string>
#include <glm/gtx/string_cast.hpp>
#include "Game/GameManager.h"

MinerController::MinerController(GameObject &gameObject)
    : EnemyController(gameObject)
    , currentChosenOre_(nullptr)
{
    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Idle State"),
            ActionEntry("Enter",
            [&]()
            {
                if (collider_)
                {
                    collider_->enabled = true; 
                }
                if (arrowObject_)
                {
                    arrowObject_->SetActive(false);
                }

                if (team_ == 1)
                {
                    GameManager::Instance()->SetTeamOneMoney(
                        GameManager::Instance()->GetTeamOneMoney() +
                        timeSpentMining_ * 10.0f);
                }
                else
                {
                    GameManager::Instance()->SetTeamTwoMoney(
                        GameManager::Instance()->GetTeamTwoMoney() +
                        timeSpentMining_ * 10.0f);
                }
            }),
            ActionEntry("Update",
            [&]()
            {
                scanTimer_ += stateMachine_->currentTimestep;

                if (scanTimer_ >= 2.0f)
                {
                    currentChosenOre_ = mineController_
                        ->GetRandomOreObject(&gameObject).get();
                    scanTimer_ = 0.0f;
                }
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Return State"),
            ActionEntry("Update", [&]()
            {
                Move();

                if (arrowObject_)
                {
                    arrowObject_->SetActive(true);
                    auto direction = tilemap_->LocalToWorld(targetTilemapPosition_) -
                        GetTransform().GetPosition();
                    if (glm::length(direction) > 0.05f)
                    {
                        arrowObject_->GetTransform().SetRotation(glm::quat(
                            glm::vec3(0, 0, std::atan2(direction.y, direction.x))
                        ));
                    }
                }

            }),
            ActionEntry("Enter",
            [&]()
            {
                timer_ = 0.0f;
                targetTilemapPosition_ = team_ == 1 ? glm::ivec2(0, 0) : 
                    glm::ivec2(Tilemap::MaxHorizontalLength - 1, Tilemap::MaxVerticalLength - 1);
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Mining State"),
            ActionEntry("Enter",
            [&]()
            {
                if (arrowObject_)
                {
                    arrowObject_->SetActive(false);
                }
                timeSpentMining_ = 0.0f;
            }),
            ActionEntry("Update",
            [&]()
            {
                timeSpentMining_ += stateMachine_->currentTimestep;
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Move State"), 
            ActionEntry("Update", [&]()
            {
                Move();
                if (arrowObject_ && currentChosenOre_ && currentChosenOre_->ActiveSelf())
                {
                    auto direction = currentChosenOre_->GetTransform().GetPosition() -
                        GetTransform().GetPosition();
                    arrowObject_->GetTransform().SetRotation(glm::quat(
                        glm::vec3(0, 0, std::atan2(direction.y, direction.x))
                    ));
                }
            }),
            ActionEntry("Enter", 
            [&]() 
            {
                if (arrowObject_)
                {
                    arrowObject_->SetActive(true);
                }
                targetTilemapPosition_ = 
                    tilemap_->WorldToLocal(currentChosenOre_
                        ->GetTransform().GetPosition());
                reachedOre_ = false;
                CC_TRACE(glm::to_string(targetTilemapPosition_));
            })
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Move State"),
            std::string("Return State"),
            [&]()
            {
                // If the ore becomes inactive while moving, try finding
                // a new ore, if not return
                if (!currentChosenOre_->ActiveSelf() && !reachedOre_)
                {
                    auto ore = mineController_->GetRandomOreObject(
                        &this->GetGameObject());
                    if (ore)
                    {
                        currentChosenOre_ = ore.get();
                        targetTilemapPosition_ = 
                            tilemap_->WorldToLocal(currentChosenOre_
                                ->GetTransform().GetPosition());
                        return false;
                    } 
                    currentChosenOre_ = nullptr;
                    targetTilemapPosition_ = team_ == 1 ? glm::ivec2(0, 0) : 
                        glm::ivec2(Tilemap::MaxHorizontalLength - 1, Tilemap::MaxVerticalLength - 1);
                    return true;
                }
                return false;
            })
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Move State"),
            std::string("Mining State"),
            [&]()
            {
                return localTilemapPosition_ == targetTilemapPosition_
                    && reachedOre_;
            })
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                if (currentChosenOre_ && reachedOre_)
                {
                    mineController_->ReleaseOreObject(&GetGameObject());
                    currentChosenOre_ = nullptr;
                }
                reachedOre_ = false;
            },
            std::string("Return State"),
            std::string("Idle State"),
            [&]()
            {
                return localTilemapPosition_ == targetTilemapPosition_;
            })
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Idle State"),
            std::string("Move State"),
            [&]()
            {
                return static_cast<bool>(currentChosenOre_);
            }
        )
    );

    stateMachine_->AddTransition(
        CreateScope<TimedTransition<>>(
            std::string("Mining State"),
            std::string("Return State"),
            5.0f 
        )
    );
    
    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                runMessage_ = false; 
            },
            std::string("Mining State"),
            std::string("Return State"),
            [&]()
            {
                return runMessage_; 
            })
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                runMessage_ = false; 
            },
            std::string("Move State"),
            std::string("Return State"),
            [&]()
            {
                return runMessage_; 
            })
    );

    stateMachine_->InitateStartState("Idle State");
}

void MinerController::Start()
{
    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
    currentTargetPosition_ = GetTransform().GetPosition();
    collider_ = GetGameObject().GetComponent<BoxCollider2D>(); 
    isGuarded_ = false;
}

void MinerController::Update(Timestep ts)
{
    stateMachine_->Update(ts);
}

void MinerController::Message(std::string message)
{
    if (message == "Run" && gameObject_.ActiveSelf())
    {
        currentHealth_ -= 20.0f;
        if (currentHealth_ <= 0.0f)
        {
            OnDeath();
        }
        runMessage_ = true;
    }
    else if (message == "Guard")
    {
        isGuarded_ = true;
    }
    else if (message == "Bandit Shoot")
    {
        currentHealth_ -= 10.0f;
        if (currentHealth_ <= 0.0f)
        {
            OnDeath();
        }
        runMessage_ = true;
    }
}

void MinerController::OnDeath()
{
    EnemyController::OnDeath();
    if (team_ == 1)
    {
        GameManager::Instance()->SetTeamOneMiners(
            GameManager::Instance()->GetTeamOneMiners() - 1
        );
    }
    else
    {
        GameManager::Instance()->SetTeamTwoMiners(
            GameManager::Instance()->GetTeamTwoMiners() - 1
        );
    }
    if (currentChosenOre_ && reachedOre_)
    {
        mineController_->ReleaseOreObject(&GetGameObject());
        currentChosenOre_ = nullptr;
    }
}

void MinerController::OnTriggerEnter2D(Collider2D *other)
{
    if (!reachedOre_ && currentChosenOre_ && &other->GetGameObject() == currentChosenOre_
        && currentChosenOre_->ActiveSelf())
    {
        mineController_->GetOreObject(&this->GetGameObject(),
            &other->GetGameObject());
        reachedOre_ = true;
        other->GetGameObject().SetActive(false);
    }
}

MinerController *MinerController::SetTilemap(Ref<Tilemap> tilemap)
{
    EnemyController::SetTilemap(tilemap);
    return this;
}

MinerController* MinerController::SetMineController(Ref<MineController> mineController)
{
    mineController_ = mineController;
    return this;
}

void MinerController::GenerateNewLocation()
{
    EnemyController::GenerateNewLocation();

}

void MinerController::Reset()
{
    EnemyController::Reset();
    timer_ = 0.0f;
    isGuarded_ = false;
    currentChosenOre_ = nullptr;
}

void MinerController::SetRestPlace(const glm::ivec2& location)
{
    restPlace_ = location;
}