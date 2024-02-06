#include "GameManager.h"
#include "Events/ApplicationEvent.h"
#include <limits>

GameManager::GameManager()
    : state_(GameState::PlayerTurn)
{

}

auto GameManager::ChangeState(GameState state) -> void
{
    if (state_ == state) return;
    GameStateChangeEvent event(state_, state);
    eventCallback_(event);
    state_ = state;

    if (state_ == GameState::MenuLevel)
    {
    }
}

void GameManager::OnTurnFinish()
{
    if (entityQueue_.empty())
    {
        return;
    }

    for (auto& entity : entityQueue_)
    {
        entity->UpdateNearbyTilesVisibility();
    }

    auto entity = entityQueue_.front();
    entityQueue_.pop_front();
    entityQueue_.push_back(entity);
    entityQueue_.front()->StartTurn();
}

void GameManager::StartGame()
{
    if (entityQueue_.empty())
    {
        return;
    }

    entityQueue_.front()->StartTurn();
}

void GameManager::NewGame()
{
}

std::vector<Ref<TilemapEntity>> GameManager::QueryTiles(
    const std::vector<glm::ivec2>& tilemapLocations)
{
    std::vector<Ref<TilemapEntity>> entities;
    for (auto& location : tilemapLocations)
    {
        for (auto& entity : entityQueue_)
        {
            if (entity->GetTilemapPosition() == location)
            {
                entities.push_back(entity);
            }
        }
    }

    return entities;
}

void GameManager::AddTilemapEntity(Ref<TilemapEntity> tilemapEntity)
{
    glm::vec4 randomColor{};
    for (int i = 0; i < 3; ++i)
    {
        randomColor[i] = (rand() % 256) / 255.0f;
    }
    randomColor.a = 1.0f;

    tilemapEntity->SetColorRepresentation(randomColor);
    entityQueue_.push_back(tilemapEntity);
}
