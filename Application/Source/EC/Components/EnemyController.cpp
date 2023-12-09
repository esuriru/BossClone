#include "EnemyController.h"

#include "Utils/Input.h"

#include "Core/KeyCodes.h"
#include "Core/Core.h"

#include "EC/Components/Transform.h"
#include "EC/Components/Collider2D.h"
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
{
    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Idle State")
        )
    );

    stateMachine_->AddState(
        CreateScope<State<>>(
            std::string("Other State"), 
            ActionEntry("Update", 
            [&]() 
            {
                static float timer_ = 0.0f;
                timer_ += stateMachine_->currentTimestep;

                if (timer_ >= 1.0f)
                {
                    // localTilemapPosition_ += glm::ivec2(1, 1);
                    auto randomDirection = GetRandomDirection();
                    if (randomDirection == EightWayDirection::None)
                    {
                        CC_ERROR("Direction None");
                        timer_ -= 1.0f;
                        return;
                    }
                    CC_ASSERT(fabs(Utility::ConvertDirection(randomDirection).x) < 2, "Test");
                    localTilemapPosition_ += Utility::ConvertDirection(randomDirection);
                    CC_TRACE(glm::to_string(localTilemapPosition_));
                    GetTransform().SetPosition(tilemap_->LocalToWorld(localTilemapPosition_));
                    timer_ -= 1.0f;
                }
            }),
            ActionEntry("Enter", 
            [&]() 
            {
                CC_TRACE("Test");
            })
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Idle State"),
            std::string("Other State"),
            [&]()
            {
                return Input::Instance()->IsKeyPressed(Key::B);
            })
    );

    stateMachine_->AddTransition(
        CreateScope<TimedTransition<>>(
            std::string("Idle State"),
            std::string("Other State"),
            5.0f 
        )
    );

    stateMachine_->AddTransition(
        CreateScope<Transition<>>(
            std::string("Other State"),
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
}

void EnemyController::Update(Timestep ts)
{
    stateMachine_->Update(ts);
}

void EnemyController::OnTriggerEnter2D(Collider2D *other)
{

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

EightWayDirection EnemyController::GetRandomDirection()
{
    auto directions = GetPossibleDirections();

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

    EightWayDirectionFlags deltaFlags;

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
        deltaFlags |= delta.y > 0 ? EightWayDirectionFlags::Right : 
            EightWayDirectionFlags::Left;
    }
    return deltaFlags;
}
