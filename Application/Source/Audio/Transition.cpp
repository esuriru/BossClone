#include "Transition.h"
#include "Core/Core.h"
#include "Utils/MusicPlayer.h"

Transition::Transition(ISound* sound)
    : timer_(0.0f)
    , length_(0.0f)
    , sound_(sound)
    , done_(false)
{
    Init();
}

Transition::Transition(ISound* sound, float length)
    : timer_(0.0f)
    , length_(length)
    , sound_(sound)
    , done_(false)
{
    Init();
}

void Transition::Update(float dt)
{
    switch (currentState_)
    {
        case Enter:
            OnEnter(dt);
            break;
        case Transit:
            OnTransit(dt);
            break;
        case Exit:
            OnExit(dt);
            break;
    }
}

void Transition::Init()
{
    currentState_ = TransitionState::Enter;
}

bool Transition::IsDone()
{
    return done_;
}

FadeInTransition::FadeInTransition(ISound* sound, float lengthSeconds, float defaultVolume)
    : Transition(sound, lengthSeconds)
    , inverseLength_(1.0f / lengthSeconds)
    , defaultVolume_(defaultVolume)
{

}

FadeOutTransition::FadeOutTransition(ISound *sound, float lengthSeconds, float offset, OnExitCallback callback)
    : Transition(sound, lengthSeconds)
    , inverseLength_(1.0f / lengthSeconds)
    , defaultVolume_(sound->getVolume())
    , callback_(callback)
{
    timer_ = 0.0f;
}

void FadeInTransition::OnTransit(float dt)
{
    Transition::OnTransit(dt);
    float value = inverseLength_ * timer_ * defaultVolume_;

    // CC_TRACE(value);

    sound_->setVolume(value);
    if (timer_ >= length_)
    {
        currentState_ = Exit;
    }
}

void FadeOutTransition::OnExit(float dt)
{
    Transition::OnExit(dt);
    callback_(sound_);
}

void FadeOutTransition::OnTransit(float dt)
{
    timer_ -= dt;
    float value = inverseLength_ * timer_ * defaultVolume_;

    sound_->setVolume(value);
    if (timer_ <= 0)
    {
        currentState_ = Exit;
    }
}

void FadeOutTransition::OnEnter(float dt)
{
    if (offset_ > 0.0f)
    {
        if (timer_ <= 0.0f)
        {
            timer_ += offset_;
        }
        timer_ -= dt;
        if (timer_ <= 0.0f)
        {
            timer_ = length_;
            defaultVolume_ = sound_->getVolume();
            currentState_ = Transit;
        }
    }
    else
    {
        timer_ = length_;
        defaultVolume_ = sound_->getVolume();
        currentState_ = Transit;
    }
}
