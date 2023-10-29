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

private:
    std::vector<Ref<GameObject>> oreObjects_; 
    std::unordered_map<GameObject*, Ref<GameObject>> minerToOreMap_;

};