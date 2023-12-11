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
#include <glm/gtx/compatibility.hpp>

EnemyController::EnemyController(GameObject &gameObject)
    : Component(gameObject)
    , stateMachine_(CreateScope<StateMachine<>>("Default"))
{
}

void EnemyController::Start()
{
    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
    currentTargetPosition_ = GetTransform().GetPosition();
    collider_ = GetGameObject().GetComponent<BoxCollider2D>(); 

    if (arrowObject_)
    {
        arrowObject_->GetTransform().SetPosition(
            GetTransform().GetPosition());
    }
}

void EnemyController::Update(Timestep ts)
{
    stateMachine_->Update(ts);
}

void EnemyController::FixedUpdate(float fixedDeltaTime)
{
    stateMachine_->FixedUpdate(fixedDeltaTime);
}

void EnemyController::Reset()
{
    timer_ = 0.0f;
    stateMachine_->Reset();
}

void EnemyController::Move()
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
    if (timer_ >= 1.0f)
    {
        localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
        if (localTilemapPosition_ == targetTilemapPosition_)
        {
            return;
        }

        GenerateNewLocation();
        timer_ = 0.0f;
    }
}

EnemyController* EnemyController::SetTilemap(Ref<Tilemap> tilemap)
{
    tilemap_ = tilemap;
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

void EnemyController::SetBounds(glm::ivec2 min, glm::ivec2 max)
{
    boundsMin_ = min;
    boundsMax_ = max;
}

void EnemyController::SetPool(Ref<EnemyPool> pool)
{
    pool_ = pool;
}

std::string EnemyController::GetCurrentStateName()
{
    return stateMachine_->GetCurrentStateName(); 
}

bool EnemyController::InBounds(glm::ivec2 coords) const
{
    return (coords.x >= boundsMin_.x && 
        coords.y >= boundsMin_.y && 
        coords.x < boundsMax_.x &&
        coords.y < boundsMax_.y);
}

bool EnemyController::InBounds(int x, int y) const
{
    return (x >= boundsMin_.x && 
        y >= boundsMin_.y && 
        x < boundsMax_.x &&
        y < boundsMax_.y);
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
    return GetPossibleDirections(localTilemapPosition_);
}

EightWayDirectionFlags EnemyController::GetPossibleDirections(glm::ivec2 position)
{
    EightWayDirectionFlags flags = static_cast<EightWayDirectionFlags>(0);

    bool upPossible = InBounds(position.x, 
        position.y + 1); 
    bool rightPossible = InBounds(position.x + 1, 
        position.y); 
    bool downPossible = InBounds(position.x, 
        position.y - 1); 
    bool leftPossible = InBounds(position.x - 1, 
        position.y); 

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
