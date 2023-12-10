#include "EnemyController.h"

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

#include "Tilemap.h"

#include <string>
#include <glm/gtx/string_cast.hpp>

EnemyController::EnemyController(GameObject &gameObject)
    : Component(gameObject)
    , stateMachine_(CreateScope<StateMachine<>>("Default"))
    , currentChosenOre_(nullptr)
{
    auto moveToTarget = [&]()
    {
        timer_ += stateMachine_->currentTimestep;

        GetTransform().SetPosition(
            Utility::Lerp(GetTransform().GetPosition(),
            currentTargetPosition_,
            timer_));
        if (timer_ >= 1.0f)
        {
            localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
            if (localTilemapPosition_ == targetTilemapPosition_)
            {
                return;
            }

            GenerateNewLocation();
            timer_ -= 1.0f;
        }
    };

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
            }),
            ActionEntry("Update",
            [&]()
            {
                scanTimer_ += stateMachine_->currentTimestep;

                if (scanTimer_ >= 2.0f)
                {
                    currentChosenOre_ = mineController_
                        ->GetRandomOreObject(&gameObject).get();
                    scanTimer_ -= 2.0f;
                }
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Return State"),
            ActionEntry("Update", moveToTarget),
            ActionEntry("Enter",
            [&]()
            {
                timer_ = 0.0f;
                targetTilemapPosition_ = { 0, 0 };
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Mining State"),
            ActionEntry("Enter",
            [&]()
            {
                CC_TRACE("Mining");
            })
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Move State"), 
            ActionEntry("Update", moveToTarget),
            ActionEntry("Enter", 
            [&]() 
            {
                targetTilemapPosition_ = 
                    tilemap_->WorldToLocal(currentChosenOre_
                        ->GetTransform().GetPosition());
                reachedOre_ = false;
                // if (mineController_ && tilemap_)
                // {
                //     auto oreObject = mineController_
                //         ->GetRandomOreObject(&gameObject);
                //     if (oreObject)
                //     {
                //         currentChosenOre_ = oreObject;
                //     }
                //     else
                //     {
                //         currentChosenOre_ = nullptr;
                //     }
                // }
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
                    targetTilemapPosition_ = { 0, 0 };
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
            std::string("Idle State"),
            std::string("Move State"),
            [&]()
            {
                return Input::Instance()->IsKeyPressed(Key::B);
            })
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            [&]()
            {
                if (currentChosenOre_)
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
            std::string("Move State"),
            std::string("Idle State"),
            [&]()
            {
                return Input::Instance()->IsKeyPressed(Key::V);
            })
    );

    stateMachine_->InitateStartState("Idle State");
}

void EnemyController::Start()
{
    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
    currentTargetPosition_ = GetTransform().GetPosition();
    collider_ = GetGameObject().GetComponent<BoxCollider2D>(); 
}

void EnemyController::Update(Timestep ts)
{
    stateMachine_->Update(ts);
}

void EnemyController::OnTriggerEnter2D(Collider2D *other)
{
    if (currentChosenOre_ && &other->GetGameObject() == currentChosenOre_)
    {
        mineController_->GetOreObject(&this->GetGameObject(),
            &other->GetGameObject());
        reachedOre_ = true;
        other->GetGameObject().SetActive(false);
    }
}

EnemyController* EnemyController::SetTilemap(Ref<Tilemap> tilemap)
{
    tilemap_ = tilemap;
    return this;
}

EnemyController* EnemyController::SetMineController(Ref<MineController> mineController)
{
    mineController_ = mineController;
    return this;
}

void EnemyController::GenerateNewLocation()
{
    auto randomDirection = GetRandomDirection(GetNextTargetDirections());
    if (randomDirection == EightWayDirection::None)
    {
        CC_ERROR("Direction None");
        return;
    }
    currentTargetPosition_ = tilemap_->LocalToWorld(localTilemapPosition_
        + Utility::ConvertDirection(randomDirection));
}

EightWayDirection EnemyController::GetRandomDirection(EightWayDirectionFlags flags)
{
    auto directions = flags; 

    uint32_t directionCount = 0;
    EightWayDirection setBitDirections[8] {};

    for (int i = static_cast<int>(EightWayDirectionFlags::Up); 
        i <= (static_cast<int>(EightWayDirectionFlags::Up_Left) << 1); i <<= 1)
    {
        if (static_cast<bool>(directions & 
            static_cast<EightWayDirectionFlags>(i))) 
        {
            setBitDirections[directionCount] = 
                static_cast<EightWayDirection>(i);
            ++directionCount;
        }
    }

    return directionCount == 0 ? EightWayDirection::None :
        setBitDirections[static_cast<uint32_t>(Random::Range(0,
        static_cast<int>(directionCount - 1)))];

}

EightWayDirectionFlags EnemyController::GetPossibleDirections()
{
    EightWayDirectionFlags flags = static_cast<EightWayDirectionFlags>(0);

    bool upPossible = Tilemap::InBounds(localTilemapPosition_.x, 
        localTilemapPosition_.y + 1); 
    bool rightPossible = Tilemap::InBounds(localTilemapPosition_.x + 1, 
        localTilemapPosition_.y); 
    bool downPossible = Tilemap::InBounds(localTilemapPosition_.x, 
        localTilemapPosition_.y - 1); 
    bool leftPossible = Tilemap::InBounds(localTilemapPosition_.x - 1, 
        localTilemapPosition_.y); 

    std::array<std::pair<EightWayDirectionFlags, bool>, 8> conditions
    {
        { 
            { EightWayDirectionFlags::Up, upPossible },
            { EightWayDirectionFlags::Up_Right, upPossible && rightPossible },
            { EightWayDirectionFlags::Right, rightPossible },
            { EightWayDirectionFlags::Down_Right, downPossible && rightPossible },
            { EightWayDirectionFlags::Down, downPossible },
            { EightWayDirectionFlags::Down_Left, downPossible && leftPossible },
            { EightWayDirectionFlags::Left, leftPossible },
            { EightWayDirectionFlags::Up_Left, upPossible && leftPossible },
        }
    };

    for (auto& pair : conditions)
    {
        if (pair.second)
        {
            flags |= pair.first;
        }
    }

    return flags;
}

EightWayDirectionFlags EnemyController::GetNextTargetDirections()
{
    auto flags = GetPossibleDirections();
    auto delta = targetTilemapPosition_ - localTilemapPosition_;

    EightWayDirectionFlags deltaFlags = static_cast<EightWayDirectionFlags>(0);

    if (delta.x > 0)
    {
        if (delta.y > 0)
        {
            deltaFlags |= EightWayDirectionFlags::Up | EightWayDirectionFlags::Right | 
                EightWayDirectionFlags::Up_Right;
        }
        else if (delta.y < 0)
        {
            deltaFlags |= EightWayDirectionFlags::Down | EightWayDirectionFlags::Right | 
                EightWayDirectionFlags::Down_Right;
        }
        else
        {
            deltaFlags |= EightWayDirectionFlags::Right;
        }
    }
    else if (delta.x < 0)
    {
        if (delta.y > 0)
        {
            deltaFlags |= EightWayDirectionFlags::Up | EightWayDirectionFlags::Left | 
                EightWayDirectionFlags::Up_Left;
        }
        else if (delta.y < 0)
        {
            deltaFlags |= EightWayDirectionFlags::Down | EightWayDirectionFlags::Left | 
                EightWayDirectionFlags::Down_Left;
        }
        else
        {
            deltaFlags |= EightWayDirectionFlags::Left;
        }
    }
    else if (delta.y != 0)
    {
        deltaFlags |= delta.y > 0 ? EightWayDirectionFlags::Up : 
            EightWayDirectionFlags::Down;
    }
    return flags & deltaFlags;
}
