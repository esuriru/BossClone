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
#include "BanditController.h"
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
        GameManager::Instance()->PollPurchases();
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
            if (collider->GetGameObject().CompareTag("Miner"))
            {
                enemyName_ = "Miner";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<MinerController>();
                UpdateText();
                return;
            }
            else if (collider->GetGameObject().CompareTag("Witch"))
            {
                enemyName_ = "Witch";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<WitchController>();
                UpdateText();
                return;
            }
            else if (collider->GetGameObject().CompareTag("Knight"))
            {
                enemyName_ = "Knight";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<KnightController>();
                UpdateText();
                return;
            }
            else if (collider->GetGameObject().CompareTag("Bandit"))
            {
                enemyName_ = "Bandit";
                lastClickedEnemy_ = 
                    collider->GetGameObject().GetComponent<BanditController>();
                UpdateText();
                return;
            }
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

    if (GameManager::Instance()->spawnMiner.first)
    {
        auto enemy = minerPool_->Get();
        auto team = GameManager::Instance()->spawnMiner.second;
        if (team == 1)
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(0, 0));
        }
        else
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(
                Tilemap::MaxHorizontalLength - 1, Tilemap::MaxVerticalLength - 1));
        }
        enemy->SetTeam(team);
        enemy->Init();
        GameManager::Instance()->spawnMiner.first = false;
    }

    if (GameManager::Instance()->spawnWitch.first)
    {
        auto enemy = witchPool_->Get();
        auto team = GameManager::Instance()->spawnWitch.second;
        if (team == 1)
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(2, 12));
        }
        else
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(
                Tilemap::MaxHorizontalLength - 3, 12));
        }
        enemy->SetTeam(team);
        enemy->Init();
        GameManager::Instance()->spawnWitch.first = false;
    }

    if (GameManager::Instance()->spawnKnight.first)
    {
        auto enemy = knightPool_->Get();
        auto team = GameManager::Instance()->spawnKnight.second;
        if (team == 1)
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(3, 4));
        }
        else
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(21, 21));
        }
        enemy->SetTeam(team);
        enemy->Init();
        GameManager::Instance()->spawnKnight.first = false;
    }

    if (GameManager::Instance()->spawnBandit.first)
    {
        auto enemy = banditPool_->Get();
        auto team = GameManager::Instance()->spawnBandit.second;
        if (team == 1)
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(12, 16));
        }
        else
        {
            enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(12, 8));
        }
        enemy->SetTeam(team);
        enemy->Init();
        GameManager::Instance()->spawnBandit.first = false;
    }

    
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
                enemy->SetTeam(currentSelectedTeam_);
                enemy->GetTransform().SetPosition(tilemap_->LocalToWorld(coords));
            }
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
    TextCentred("Team: " + enemyTeam_);
    TextCentred("Current Health: " + enemyCurrentHealth_);
    TextCentred("Current State: " + enemyCurrentState_);
    ImGui::Spacing();

    if (ImGui::Button(currentSelectedTeam_ == 1 ? "Team 1" : "Team 2")) 
    {
        currentSelectedTeam_ = currentSelectedTeam_ == 1 ? 2 : 1;
    }

    ImGui::Spacing();
    TextCentred("1 - Miner");
    TextCentred("2 - Knight");
    TextCentred("3 - Witch");
    TextCentred("4 - Bandit");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    TextCentred("Team 1: $" + std::to_string(GameManager::Instance()->GetTeamOneMoney()));
    TextCentred("Team 2: $" + std::to_string(GameManager::Instance()->GetTeamTwoMoney()));
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
    enemyCurrentHealth_ = std::to_string(lastClickedEnemy_->GetHealth());
    enemyTeam_ = std::to_string(lastClickedEnemy_->GetTeam());
    enemyCurrentState_ = lastClickedEnemy_->GetCurrentStateName();
}
