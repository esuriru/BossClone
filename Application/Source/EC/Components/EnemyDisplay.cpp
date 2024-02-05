#include "EnemyDisplay.h"

#include "Physics/PhysicsWorld.h"
#include "Utils/Input.h"

#include "Game/EnemyPool.h"

#include "EC/GameObject.h"

#include "Core/KeyCodes.h"

#include <imgui.h>
#include "Scene/SceneManager.h"
#include "Core/Application.h"

#include "Game/GameManager.h"

EnemyDisplay::EnemyDisplay(GameObject &gameObject)
    : Component(gameObject)
    , chosenPool_(nullptr)
    , enemyTeam_("")
{
}

void EnemyDisplay::Update(Timestep ts)
{
    timer_ += ts;
    if (timer_ >= 5.0f)
    {
        timer_ = 0.0f;
    }
    static bool lmbDown = false;
    static bool rmbDown = false;

    auto mouseCoords = Input::Instance()->GetMouseCoords();
    if (Input::Instance()->GetMouseButtonUp(0))
    {
        lmbDown = false;
    }
    if (Input::Instance()->GetMouseButtonUp(1))


    {
        rmbDown = false;
    }

    if (Input::Instance()->GetMouseButtonDown(0) && !lmbDown)
    {
        lmbDown = true;
        auto colliders = PhysicsWorld::Instance()->RaycastScreen(mouseCoords);
        for (auto collider : colliders)
        {
        }
    }

    if (Input::Instance()->IsKeyPressed(Key::D1))
    {
        CC_TRACE("Spawn Miner");
        // auto miner = minerPool_->Get();
        chosenPool_ = minerPool_;
        min_ = minerBoundsMin_;
        max_ = minerBoundsMax_;
    }
    else if (Input::Instance()->IsKeyPressed(Key::D2))
    {
        CC_TRACE("Spawn Knight");
        // auto miner = minerPool_->Get();
        chosenPool_ = knightPool_;
        min_ = knightBoundsMin_;
        max_ = knightBoundsMax_;
    }
    else if (Input::Instance()->IsKeyPressed(Key::D3))
    {
        CC_TRACE("Spawn Witch");
        // auto miner = minerPool_->Get();
        chosenPool_ = witchPool_;
        min_ = witchBoundsMin_;
        max_ = witchBoundsMax_;
    }
    else if (Input::Instance()->IsKeyPressed(Key::D4))
    {
        CC_TRACE("Spawn Bandit");
        // auto miner = minerPool_->Get();
        chosenPool_ = banditPool_;
        min_ = banditBoundsMin_;
        max_ = banditBoundsMax_;
    }


    auto camera = SceneManager::Instance()->GetActiveScene()->GetCamera();
    if (!camera)
    {
        return;
    }

    float halfScreenWidth = static_cast<float>(Application::Instance()->GetWindowWidth()) *
        0.5f;
    float halfScreenHeight = static_cast<float>(Application::Instance()->GetWindowHeight()) *
        0.5f;
    glm::vec4 worldSpaceMouse = glm::inverse(camera->GetViewProjectionMatrix()) * 
        glm::vec4((mouseCoords.x - halfScreenWidth) / halfScreenWidth, 
        -(mouseCoords.y - halfScreenHeight) / halfScreenHeight, -1, 1);
    glm::vec3 worldMousePos = worldSpaceMouse;

    if (tilemap_ && chosenPool_)
    {
        auto coords = tilemap_->WorldToLocal(worldMousePos);
        if (InBounds(coords))
        {
            if (Input::Instance()->GetMouseButtonDown(1) && !rmbDown)
            {
                rmbDown = true;
                
                auto enemy = chosenPool_->Get();
                CC_TRACE("Spawn");
                // enemy->SetTeam(currentSelectedTeam_);
                // enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(coords));
            }
        }
    }
}

void EnemyDisplay::OnImGuiRender()
{  
    if (lastClickedEnemy_)
    {
        // enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
        // enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
    }

    auto io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 300, io.DisplaySize.y * 0.5f - 150));
    ImGui::SetNextWindowBgAlpha(0.2f);


    ImGui::Begin("Display", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    static GameManager* gameManager = GameManager::Instance();
    auto& entities = gameManager->GetAllEntities();
    const ImVec2 p = ImGui::GetCursorScreenPos(); 
    static float sz = 36.0f; 
    static float thickness = 4.0f; 

    float oldSize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 2;
    ImGui::PushFont(ImGui::GetFont());
    auto textSize = ImGui::CalcTextSize("Entities");
    TextCentred("Entities");

    ImGui::GetFont()->Scale = oldSize; 
    ImGui::PopFont();

    float x = p.x + 4.0f, 
        y = p.y + 4.0f + textSize.y,  
        spacing = 8.0f; 
        
    for (size_t i = 0; i < entities.size(); ++i)
    {
        auto entityColor = entities[i]->GetColourRepresentation();

        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x+sz, y+sz), 
            ImColor(entityColor.r, entityColor.g, entityColor.b, entityColor.a), 
            10.0f, 
            ImDrawCornerFlags_All); 
        
        x += sz+spacing;
        y += 10; 
        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::Text("HP : %.2f", entities[i]->GetHealth());
        y -= 10; 
        x = p.x + 4; 

        y += sz+spacing; 
    }
    // TextCentred("Name: " + enemyName_);
    // TextCentred("Team: " + enemyTeam_);
    // TextCentred("Current Health: " + enemyCurrentHealth_);
    // TextCentred("Current State: " + enemyCurrentState_);
    // ImGui::Spacing();

    // if (ImGui::Button(currentSelectedTeam_ == 1 ? "Team 1" : "Team 2")) 
    // {
    //     currentSelectedTeam_ = currentSelectedTeam_ == 1 ? 2 : 1;
    // }

    // ImGui::Spacing();
    // TextCentred("1 - Miner");
    // TextCentred("2 - Knight");
    // TextCentred("3 - Witch");
    // TextCentred("4 - Bandit");
    // ImGui::Spacing();
    // ImGui::Spacing();
    // ImGui::Spacing();
    // TextCentred("Team 1: $" + std::to_string(GameManager::Instance()->GetTeamOneMoney()));
    // TextCentred("Team 2: $" + std::to_string(GameManager::Instance()->GetTeamTwoMoney()));
    ImGui::End();
   
}

void EnemyDisplay::Init(Ref<Tilemap> tilemap, Ref<EnemyPool> minerPool, Ref<EnemyPool> knightPool, Ref<EnemyPool> witchPool, Ref<EnemyPool> banditPool)
{
    tilemap_ = tilemap;
    minerPool_ = minerPool;
    knightPool_ = knightPool;
    witchPool_ = witchPool;
    banditPool_ = banditPool;
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

void EnemyDisplay::AddWitchToPool(Ref<EnemyController> witch)
{
    if (witchPool_)
    {
        witchPool_->Add(witch);
    }
}

void EnemyDisplay::AddBanditToPool(Ref<EnemyController> bandit)
{
    if (banditPool_)
    {
        banditPool_->Add(bandit);
    }
}

bool EnemyDisplay::InBounds(glm::ivec2 coords)
{
    return (coords.x >= min_.x && 
        coords.y >= min_.y && 
        coords.x < max_.x &&
        coords.y < max_.y);
}

void EnemyDisplay::TextCentred(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.data()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.data());
}

void EnemyDisplay::TextCentred(float value)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(std::to_string(value).data()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%.2f", value);
}

void EnemyDisplay::UpdateText()
{
    // enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
    // enemyTeam_ = std::to_string(lastClickedEnemy_->GetTeam());
    // enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
}
