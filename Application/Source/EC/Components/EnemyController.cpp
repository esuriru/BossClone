#include "EnemyController.h"

#include "Utils/Input.h"

#include "Core/KeyCodes.h"
#include "Core/Core.h"

#include "EC/Components/Transform.h"
#include "EC/Components/Collider2D.h"

#include "Game/StateMachine.h"
#include "Game/ActionEntry.h"

#include <string>

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
                    ++localTilemapPosition_.first;
                    ++localTilemapPosition_.second;
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
    if (Input::Instance()->IsKeyDown(Key::L))
    {
        // ++localTilemapPosition_.first;
        // GetTransform().SetPosition(tilemap_->LocalToWorld(localTilemapPosition_));
        GetTransform().Translate(glm::vec3(ts * 5.0f, 0, 0));
    }
    if (Input::Instance()->IsKeyDown(Key::I))
    {
        GetTransform().Translate(glm::vec3(0, ts * 5.0f, 0));
    }
    if (Input::Instance()->IsKeyDown(Key::J))
    {
        GetTransform().Translate(glm::vec3(-ts * 5.0f, 0, 0));
    }
    if (Input::Instance()->IsKeyDown(Key::K))
    {
        GetTransform().Translate(glm::vec3(0, -ts * 5.0f, 0));
    }
    stateMachine_->Update(ts);
}

void EnemyController::OnTriggerEnter2D(Collider2D *other)
{
    CC_TRACE("Trigger enter 2D");
}

void EnemyController::SetTilemap(Ref<Tilemap> tilemap)
{
    tilemap_ = tilemap;
}
