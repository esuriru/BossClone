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

void GameManager::AddTilemapEntity(Ref<TilemapEntity> tilemapEntity)
{
    entityQueue_.push_back(tilemapEntity);
}
