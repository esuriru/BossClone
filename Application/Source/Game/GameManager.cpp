#include "GameManager.h"
#include "Events/ApplicationEvent.h"
#include <limits>
#include "Utils/MusicPlayer.h"
#include "Core/Core.h"
#include <includes/ik_ISoundEffectControl.h>

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

    switch (state_)
    {
        case GameState::MenuLevel:
            if (timer_ < bestTime_)         
            {
                bestTime_ = timer_;
            }
            break;
        case GameState::PlayingLevel:
            {
                auto musicPlayer = MusicPlayer::Instance();
                musicPlayer->SetPlayMode(MusicPlayer::PLAYMODE::SINGLE_LOOP);
                auto music = musicPlayer->PlayMusic();
                // music->GetSound()->
                musicPlayer->AddTransition(CreateScope<FadeInTransition>(musicPlayer->GetCurrentSound(), 4.0f));
                musicPlayer->AddTransition(CreateScope<FadeOutTransition>(musicPlayer->GetCurrentSound(), 4.0f, 20.f));
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
