#include "Utils/SoundInfo.h"
#include "Core/Core.h"

class Transition
{
public:
    Transition() = default;
    Transition(ISound* sound);
    Transition(ISound* sound, float length);
    void Update(float dt);

    void Init();

    bool IsDone();

protected:
    ISound* sound_;
    enum TransitionState
    {
        Enter,
        Transit,
        Exit
    };

    virtual void OnEnter(float dt)
    {
        timer_ = 0.0f;
        currentState_ = TransitionState::Transit;
    }
     
    virtual void OnTransit(float dt)
    {
        timer_ += dt;
    }

    virtual void OnExit(float dt)
    {
        done_ = true;
    }

    TransitionState currentState_;
    float length_, timer_;
    bool done_;

};

class FadeInTransition : public Transition
{
public: 
    FadeInTransition() = default;
    FadeInTransition(ISound* sound, float lengthSeconds = 1.0f, float defaultVolume = 1.0f);

private:
    float defaultVolume_;
    float inverseLength_;

    void OnTransit(float dt) override;

};

class FadeOutTransition : public Transition
{
public: 
    FadeOutTransition() = default;
    FadeOutTransition(ISound* sound, float lengthSeconds = 1.0f, float offset = 0.0f);

private:
    float defaultVolume_;
    float inverseLength_;
    float offset_;

    void OnEnter(float dt) override;
    void OnTransit(float dt) override;

};