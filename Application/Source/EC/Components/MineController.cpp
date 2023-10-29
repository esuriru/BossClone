#include "MineController.h"

#include <glm/gtc/random.hpp>
#include "Utils/Util.h"

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
    
    auto& oreObject = it->second; 
    minerToOreMap_.erase(miner);    

    oreObjects_.emplace_back(oreObject);
}

Ref<GameObject> MineController::GetRandomOreObject(GameObject* miner)
{
    if (oreObjects_.empty())
    {
        CC_TRACE("No more ore objects left.");
        return nullptr;
    }
    
    auto randomIndex = static_cast<uint32_t>(glm::linearRand(0, static_cast<int>(oreObjects_.size() - 1)));
    auto& oreObject = oreObjects_[randomIndex];
    Utility::RemoveAt(oreObjects_, randomIndex);

    minerToOreMap_[miner] = oreObject;
    return oreObject;
}
