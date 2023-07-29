#include "GameManager.h"
#include "Events/ApplicationEvent.h"
#include <limits>
#include "Utils/MusicPlayer.h"
#include "Core/Core.h"
#include <includes/ik_ISoundEffectControl.h>
#include "Audio/Transition.h"

GameManager::GameManager()
    : state_(GameState::MenuLevel)
    , bestTime_(std::numeric_limits<float>::max())
{
}

void GameManager::Init()
{
    auto musicPlayer = MusicPlayer::Instance();
    auto music = musicPlayer->PlayMusicByID(2);
    musicPlayer->AddTransition(CreateScope<FadeInTransition>(musicPlayer->GetCurrentSound(), 1.0f));
}

auto GameManager::ChangeState(GameState state) -> void
{
    if (state_ == state) return;
    GameStateChangeEvent event(state_, state);
    eventCallback_(event);
    state_ = state;

    switch (state_)
    {
        case GameState::MenuLevel:
            {
                auto musicPlayer = MusicPlayer::Instance();
                musicPlayer->AddTransition(CreateScope<FadeOutTransition>(musicPlayer->GetCurrentSound(), 0.5f));
            }
            if (timer_ < bestTime_)         
            {
                bestTime_ = timer_;
            }
            break;
        case GameState::PlayingLevel:
            {
                if (event.GetOldState() == GameState::Paused)
                    return;
                auto musicPlayer = MusicPlayer::Instance();
                // musicPlayer->SetPlayMode(MusicPlayer::PLAYMODE::SINGLE_LOOP);
                musicPlayer->AddTransition(CreateScope<FadeOutTransition>(musicPlayer->GetCurrentSound(), 0.5f));
                auto music = musicPlayer->PlayMusicByID(1);
                musicPlayer->AddTransition(CreateScope<FadeInTransition>(musicPlayer->GetCurrentSound(), 4.0f));
                // musicPlayer->AddTransition(CreateScope<FadeOutTransition>(musicPlayer->GetCurrentSound(), 4.0f, 10.f));
            }
            break;
        default:
            break;
    }
}

auto GameManager::UploadTime(float time) -> void
{
    timer_ = time;
}
