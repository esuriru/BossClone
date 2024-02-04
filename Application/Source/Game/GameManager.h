#pragma once
#include "Events/EventDispatcher.h"
#include "Utils/Singleton.h"
#include "Core/Window.h"

#include "EC/Components/TilemapEntity.h"

#include <queue>
#include <deque>

enum GameState
{
    PlayerTurn,
    EnemyTurn,
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

    void AddTilemapEntity(Ref<TilemapEntity> tilemapEntity);
    void OnTurnFinish();
    void StartGame();

private:
    GameState state_;
    EventCallback eventCallback_;

    std::deque<Ref<TilemapEntity>> entityQueue_;
};