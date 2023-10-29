#include "Component.h"
#include "GameObject.h"
#include "Components/Transform.h"

#include "Scene/SceneManager.h"

Transform &Component::GetTransform()
{
    return gameObject_.GetTransform();
}

Ref<GameObject> Component::FindGameObjectByTag(const std::string& tag)
{
    return SceneManager::Instance()->GetActiveScene()->FindGameObjectByTag(tag);
}
