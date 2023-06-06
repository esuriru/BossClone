#pragma once
#include "Events/EventDispatcher.h"
#include "Utils/Singleton.h"
#include "Core/Window.h"

enum GameState
{
    PlayingLevel,
    MenuLevel,
};

class GameManager : public Utility::Singleton<GameManager>
{
public:
    GameManager();
    inline auto GetState() -> GameState { return state_; }
    auto ChangeState(GameState state) -> void;
    auto SetEventCallback(EventCallback eventCallback) -> void { eventCallback_ = eventCallback; }
    auto UploadTime(float time) -> void;
    inline auto GetBestTime() -> float { return bestTime_; }

private:
    GameState state_;
    EventCallback eventCallback_;

    float timer_;
    float bestTime_;
};