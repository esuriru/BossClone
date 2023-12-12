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

    void PollPurchases();

    inline void SetTeamOneMoney(float money)
    {
        teamOneMoney_ = money;

        // if (teamOneMiners_ < 3)
        // {
        //     if (teamOneMoney_ >= minerCost_)
        //     {
        //         teamOneMoney_ -= minerCost_;
        //         spawnMiner.first = true;
        //         spawnMiner.second = 1;
        //         teamOneMiners_++;
        //     }
        // }
        // if (teamOneWitches_ < 1)
        // {
        //     if (teamOneMoney_ >= witchCost_)
        //     {
        //         teamOneMoney_ -= witchCost_;
        //         spawnWitch.first = true;
        //         spawnWitch.second = 1;
        //         teamOneWitches_++;
        //     }
        // }
        // if (toB_ < 1)
        // {
        //     if (teamOneMoney_ >= banditCost_)
        //     {
        //         teamOneMoney_ -= banditCost_;
        //         spawnBandit.first = true;
        //         spawnBandit.second = 1;
        //         toB_++;
        //     }
        // }
        // if (toK_ < 2)
        // {
        //     if (teamOneMoney_ >= knightCost_)
        //     {
        //         teamOneMoney_ -= knightCost_;
        //         spawnKnight.first = true;
        //         spawnKnight.second = 1;
        //         toK_++;
        //     }
        // }
    }

    inline void SetTeamTwoMoney(float money)
    {
        teamTwoMoney_ = money;

        // if (teamTwoMiners_ < 3)
        // {
        //     if (teamTwoMoney_ >= minerCost_)
        //     {
        //         teamTwoMoney_ -= minerCost_;
        //         spawnMiner.first = true;
        //         spawnMiner.second = 2;
        //         teamTwoMiners_++;
        //     }
        // }
        // if (teamTwoWitches_ < 1)
        // {
        //     if (teamTwoMoney_ >= witchCost_)
        //     {
        //         teamTwoMoney_ -= witchCost_;
        //         spawnWitch.first = true;
        //         spawnWitch.second = 2;
        //         teamTwoWitches_++;
        //     }
        // }
        // if (ttB_ < 1)
        // {
        //     if (teamTwoMoney_ >= banditCost_)
        //     {
        //         teamTwoMoney_ -= banditCost_;
        //         spawnBandit.first = true;
        //         spawnBandit.second = 2;
        //         ttB_++;
        //     }
        // }
        // if (ttK_ < 2)
        // {
        //     if (teamTwoMoney_ >= knightCost_)
        //     {
        //         teamTwoMoney_ -= knightCost_;
        //         spawnKnight.first = true;
        //         spawnKnight.second = 2;
        //         ttK_++;
        //     }
        // }
    }

    inline float GetTeamTwoMoney()
    {
        return teamTwoMoney_;
    }

    inline float GetTeamOneMoney()
    {
        return teamOneMoney_;
    }

    void SetTeamOneMiners(int miners);
    int GetTeamOneMiners();
    void SetTeamTwoMiners(int miners);
    int GetTeamTwoMiners();

    void SetTeamOneWitches(int witches);
    int GetTeamOneWitches();
    void SetTeamTwoWitches(int witches);
    int GetTeamTwoWitches();

    void SetTeamOneKnights(int count);
    int GetTeamOneKnights();
    void SetTeamTwoKnights(int count);
    int GetTeamTwoKnights();

    void SetTeamOneBandits(int count);
    int GetTeamOneBandits();
    void SetTeamTwoBandits(int count);
    int GetTeamTwoBandits();

    inline auto GetState() -> GameState { return state_; }
    auto ChangeState(GameState state) -> void;
    auto SetEventCallback(EventCallback eventCallback) -> void { eventCallback_ = eventCallback; }
    auto UploadTime(float time) -> void;
    inline auto GetBestTime() -> float { return bestTime_; }

    std::pair<bool, int> spawnMiner;
    std::pair<bool, int> spawnKnight;
    std::pair<bool, int> spawnBandit;
    std::pair<bool, int> spawnWitch;

    int team;
private:
    GameState state_;
    EventCallback eventCallback_;

    const float minerCost_ = 75.0f;
    const float witchCost_ = 150.0f;
    const float knightCost_ = 250.0f;
    const float banditCost_ = 150.0f;

    float teamOneMoney_ = 0.0f;
    float teamTwoMoney_ = 0.0f;

    int teamOneMiners_ = 0;
    int teamTwoMiners_ = 0;

    int teamOneWitches_ = 0;
    int teamTwoWitches_ = 0;

    int toK_ = 0;
    int ttK_ = 0;

    int toB_ = 0;
    int ttB_ = 0;

    float timer_;
    float bestTime_;
};