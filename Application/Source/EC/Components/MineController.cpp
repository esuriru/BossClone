#include "MineController.h"

#include <glm/gtc/random.hpp>
#include "Utils/Util.h"

#include "EC/GameObject.h"

MineController::MineController(GameObject &gameObject)
    : Component(gameObject)
{
}

void MineController::AddObject(Ref<GameObject> object)
{
    oreObjects_.push_back(object);
}

void MineController::ReleaseOreObject(GameObject* miner)
{
    auto it = minerToOreMap_.find(miner);
    CC_ASSERT(it != minerToOreMap_.end(),
        "Miner not found in map.");
    
    auto oreObject = it->second; 
    minerToOreMap_.erase(miner);    

    timerMap_[oreObject] = 0.0f;
}

void MineController::GetOreObject(GameObject *miner, 
    GameObject* oreObject)
{
    for (int i = 0; i < static_cast<int>(oreObjects_.size()); ++i)
    {
        if (oreObjects_[i].get() == oreObject)
        {
            minerToOreMap_[miner] = oreObjects_[i];
            Utility::RemoveAt(oreObjects_, i);
            return;
        }
    }
}

void MineController::Update(Timestep ts)
{
    for (auto it = timerMap_.begin(); it != timerMap_.end();)
    {
        it->second += ts;

        if (it->second >= respawnTime_)
        {
            it->first->SetActive(true);
            oreObjects_.emplace_back(it->first);
            it = timerMap_.erase(it);
        }
        else 
        {
            it++;
        }
    }
}

Ref<GameObject> MineController::GetRandomOreObject(GameObject* miner)
{
    if (oreObjects_.empty())
    {
        CC_TRACE("No more ore objects left.");
        return nullptr;
    }
    
    auto randomIndex = static_cast<uint32_t>(glm::linearRand(0, static_cast<int>(oreObjects_.size() - 1)));
    auto oreObject = oreObjects_[randomIndex];

    return oreObject;
}
