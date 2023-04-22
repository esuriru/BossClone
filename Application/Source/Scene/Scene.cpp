#include "Scene.h"
#include "ECS/Coordinator.h"

static Coordinator* s_Coordinator_ = Coordinator::Instance();

Scene::Scene()
    : entities_(MaxEntities)
{

}

auto Scene::CreateEntity() -> Entity
{
    return s_Coordinator_->CreateEntity();
}



