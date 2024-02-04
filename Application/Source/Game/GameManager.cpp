#include "GameManager.h"
#include "Events/ApplicationEvent.h"
#include <limits>

GameManager::GameManager()
    : state_(GameState::PlayerTurn)
    , bestTime_(std::numeric_limits<float>::max())
{

}

void GameManager::PollPurchases()
{
    if (teamOneMiners_ < 3)
    {
        if (teamOneMoney_ >= minerCost_)
        {
            teamOneMoney_ -= minerCost_;
            spawnMiner.first = true;
            spawnMiner.second = 1;
            teamOneMiners_++;
        }
    }
    if (teamOneWitches_ < 1)
    {
        if (teamOneMoney_ >= witchCost_)
        {
            teamOneMoney_ -= witchCost_;
            spawnWitch.first = true;
            spawnWitch.second = 1;
            teamOneWitches_++;
        }
    }
    if (toB_ < 1)
    {
        if (teamOneMoney_ >= banditCost_)
        {
            teamOneMoney_ -= banditCost_;
            spawnBandit.first = true;
            spawnBandit.second = 1;
            toB_++;
        }
    }
    if (toK_ < 2)
    {
        if (teamOneMoney_ >= knightCost_)
        {
            teamOneMoney_ -= knightCost_;
            spawnKnight.first = true;
            spawnKnight.second = 1;
            toK_++;
        }
    }
    if (teamTwoMiners_ < 3)
    {
        if (teamTwoMoney_ >= minerCost_)
        {
            teamTwoMoney_ -= minerCost_;
            spawnMiner.first = true;
            spawnMiner.second = 2;
            teamTwoMiners_++;
        }
    }
    if (teamTwoWitches_ < 1)
    {
        if (teamTwoMoney_ >= witchCost_)
        {
            teamTwoMoney_ -= witchCost_;
            spawnWitch.first = true;
            spawnWitch.second = 2;
            teamTwoWitches_++;
        }
    }
    if (ttB_ < 1)
    {
        if (teamTwoMoney_ >= banditCost_)
        {
            teamTwoMoney_ -= banditCost_;
            spawnBandit.first = true;
            spawnBandit.second = 2;
            ttB_++;
        }
    }
    if (ttK_ < 2)
    {
        if (teamTwoMoney_ >= knightCost_)
        {
            teamTwoMoney_ -= knightCost_;
            spawnKnight.first = true;
            spawnKnight.second = 2;
            ttK_++;
        }
    }
}

void GameManager::OnPlayerTurnFinish()
{
    ChangeState(GameState::EnemyTurn);
}

void GameManager::SetTeamOneMiners(int miners)
{
    teamOneMiners_ = miners;

    // if (miners < 3)
    // {
    //     if (teamOneMoney_ >= minerCost_)
    //     {
    //         teamOneMoney_ -= minerCost_;
    //         spawnMiner.first = true;
    //         spawnMiner.second = 1;
    //         teamOneMiners_++;
    //     }
    // }
}

int GameManager::GetTeamOneMiners()
{
    return teamOneMiners_;
}

void GameManager::SetTeamTwoMiners(int miners)
{
    teamTwoMiners_ = miners;

    // if (miners < 3)
    // {
    //     if (teamTwoMoney_ >= minerCost_)
    //     {
    //         teamTwoMoney_ -= minerCost_;
    //         spawnMiner.first = true;
    //         spawnMiner.second = 2;
    //         teamTwoMiners_++;
    //     }
    // }
}

int GameManager::GetTeamTwoMiners()
{
    return teamTwoMiners_;
}

void GameManager::SetTeamOneWitches(int witches)
{
    teamOneWitches_ = witches;

    // if (witches < 1)
    // {
    //     if (teamOneMoney_ >= witchCost_)
    //     {
    //         teamOneMoney_ -= witchCost_;
    //         spawnWitch.first = true;
    //         spawnWitch.second = 1;
    //         teamOneWitches_++;
    //     }
    // }
}

int GameManager::GetTeamOneWitches()
{
    return teamOneWitches_;
}

void GameManager::SetTeamTwoWitches(int witches)
{
    teamTwoWitches_ = witches;

    // if (witches < 1)
    // {
    //     if (teamTwoMoney_ >= witchCost_)
    //     {
    //         teamTwoMoney_ -= witchCost_;
    //         spawnWitch.first = true;
    //         spawnWitch.second = 2;
    //         teamTwoWitches_++;
    //     }
    // }

}

int GameManager::GetTeamTwoWitches()
{
    return teamTwoWitches_;
}

void GameManager::SetTeamOneKnights(int count)
{
    toK_ = count;

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

int GameManager::GetTeamOneKnights()
{
    return toK_;
}

void GameManager::SetTeamTwoKnights(int count)
{
    ttK_ = count;

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

int GameManager::GetTeamTwoKnights()
{
    return ttK_;
}

void GameManager::SetTeamOneBandits(int count)
{
    toB_ = count;

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
}

int GameManager::GetTeamOneBandits()
{
    return toB_;
}

void GameManager::SetTeamTwoBandits(int count)
{
    ttB_ = count;

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
}

int GameManager::GetTeamTwoBandits()
{
    return ttB_;
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
