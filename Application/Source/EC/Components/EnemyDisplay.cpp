#include "EnemyDisplay.h"

#include "Physics/PhysicsWorld.h"
#include "Utils/Input.h"

#include "EC/Components/MinerController.h"
#include "EC/Components/WitchController.h"
#include "Game/EnemyPool.h"

#include "EC/GameObject.h"

#include "Core/KeyCodes.h"

#include <imgui.h>
#include "KnightController.h"

EnemyDisplay::EnemyDisplay(GameObject &gameObject)
    : Component(gameObject)
{
}

void EnemyDisplay::Update(Timestep ts)
{
    static bool mouseDown = false;
    if (Input::Instance()->GetMouseButtonUp(0))
    {
        mouseDown = false;
    }

    if (Input::Instance()->GetMouseButtonDown(0) && !mouseDown)
    {
        mouseDown = true;
        auto colliders = PhysicsWorld::Instance()->RaycastScreen(Input::Instance()->GetMouseCoords());
        for (auto collider : colliders)
        {
            if (collider->GetGameObject().CompareTag("Miner"))
            {
                enemyName_ = "Miner";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<MinerController>();
                enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
                enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
                return;
            }
            else if (collider->GetGameObject().CompareTag("Witch"))
            {
                enemyName_ = "Witch";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<WitchController>();
                enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
                enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
                return;
            }
            else if (collider->GetGameObject().CompareTag("Knight"))
            {
                enemyName_ = "Knight";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<KnightController>();
                enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
                enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
                return;
            }
        }
    }

    if (Input::Instance()->IsKeyPressed(Key::D1))
    {
        CC_TRACE("Spawn Miner");
        auto miner = minerPool_->Get();
        if (tilemap_)
        {
            miner->GetTransform().SetPosition(tilemap_->LocalToWorld(0, 0));
        }
    }
}

void EnemyDisplay::OnImGuiRender()
{  
    if (lastClickedEnemy_)
    {
        enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
        enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
    }

    auto io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 300, io.DisplaySize.y * 0.5f - 150));
    ImGui::SetNextWindowBgAlpha(0.2f);

    ImGui::Begin("Display", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);
    TextCentred("Name: " + enemyName_);
    TextCentred("Current Health: " + enemyCurrentHealth_);
    TextCentred("Current State: " + enemyCurrentState_);
    ImGui::End();
   
}

void EnemyDisplay::Init(Ref<Tilemap> tilemap, Ref<EnemyPool> minerPool, Ref<EnemyPool> knightPool)
{
    tilemap_ = tilemap;
    minerPool_ = minerPool;
}

void EnemyDisplay::AddMinerToPool(Ref<EnemyController> miner)
{
    if (minerPool_)
    {
        minerPool_->Add(miner);
    }
}

void EnemyDisplay::AddKnightToPool(Ref<EnemyController> knight)
{
    if (knightPool_)
    {
        knightPool_->Add(knight);
    }
}

void EnemyDisplay::TextCentred(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.data()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.data());
}
