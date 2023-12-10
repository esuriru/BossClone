#include "TriggerCallback.h"

#include "Collider2D.h"
#include "EC/GameObject.h"

TriggerCallback::TriggerCallback(GameObject &gameObject)
    : Component(gameObject)
{
}

void TriggerCallback::OnTriggerEnter2D(Collider2D *other)
{
    if (!callback_)
    {
        return;
    }

    switch (type_)
    {
        case Type::Tag:
        {
            if (other->GetGameObject().CompareTag(tag_))
            {
                callback_(other);
            }
        }
        default:
            break;
    }
}
