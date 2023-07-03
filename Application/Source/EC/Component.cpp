#include "Component.h"
#include "GameObject.h"
#include "Components/Transform.h"

Transform &Component::GetTransform()
{
    return gameObject_.GetTransform();
}