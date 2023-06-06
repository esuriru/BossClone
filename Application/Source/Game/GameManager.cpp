#include "GameManager.h"
#include "Events/ApplicationEvent.h"
#include <limits>

GameManager::GameManager()
    : state_(GameState::MenuLevel)
    , bestTime_(std::numeric_limits<float>::max())
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
        if (timer_ < bestTime_)         
        {
            bestTime_ = timer_;
        }
    }
}

auto GameManager::UploadTime(float time) -> void
{
    timer_ = time;
}
