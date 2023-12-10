#pragma once

#include "EC/Component.h"

#include <vector>
#include <unordered_map>

class MineController : public Component
{
public:
    MineController(GameObject& gameObject);

    void AddObject(Ref<GameObject> object);
    Ref<GameObject> GetRandomOreObject(GameObject* miner);
    void ReleaseOreObject(GameObject* miner);
    void GetOreObject(GameObject* miner, 
        GameObject* oreObject);

    void Update(Timestep ts) override;

    inline void SetRespawnTime(float time)
    {
        respawnTime_ = time;
    }

private:
    float respawnTime_ = 5.0f;
    std::vector<Ref<GameObject>> oreObjects_; 
    std::unordered_map<GameObject*, Ref<GameObject>> minerToOreMap_;
    std::unordered_map<Ref<GameObject>, float> timerMap_;

};