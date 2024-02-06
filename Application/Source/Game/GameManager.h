#pragma once
#include "Events/EventDispatcher.h"
#include "Utils/Singleton.h"
#include "Core/Window.h"

#include "EC/Components/TilemapEntity.h"
#include "EC/Components/MazeController.h"

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
    void RemoveTilemapEntity(Ref<TilemapEntity> tilemapEntity);
    void OnTurnFinish();
    void StartGame();
    void NewGame();
    void UpdateVision();
    void AttemptSpawnNewEvent(bool guaranteed = false);

    std::vector<Ref<TilemapEntity>> QueryTiles(
        const std::vector<glm::ivec2>& tilemapLocations);
    inline const std::deque<Ref<TilemapEntity>>& GetAllEntities()
    {
        return entityQueue_ ;
    }

    void SetInUpdate(bool inUpdate);

    inline bool GetEnemiesVisionEnabled() const
    {
        return enemiesVisionEnabled_;
    }

    inline void SetEnemiesVisionEnabled(bool enabled)
    {
        if (!wantsToRevealVision_)
        {
            enemiesVisionEnabled_ = enabled;
            for (auto& entity : entityQueue_)
            {
                if (entity->IsMoving())
                {
                    wantsToRevealVision_ = true;
                    return;
                }
            }

            // Since there is nothing moving, there is nothing to worry about
            UpdateVision();
        }
    } 

    inline void SetMazeController(Ref<MazeController> mazeController)
    {
        mazeController_ = mazeController;
    }

    int meteorsSpawned;

private:
    enum GameEvent
    {
        Meteor = 0,
        Count
    };

    GameState state_;
    EventCallback eventCallback_;
    bool inUpdate_ = false;
    bool wantToRestart_ = false;
    bool enemiesVisionEnabled_ = false;
    bool wantsToRevealVision_ = false;

    Ref<MazeController> mazeController_ = nullptr;
    Ref<TilemapEntity> playerEntity_ = nullptr;
    std::deque<Ref<TilemapEntity>> entityQueue_;
};