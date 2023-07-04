#include "Renderer.h"
#include "EC/GameObject.h"

Renderer::Renderer(GameObject& gameObject)
    : Component(gameObject)
    , sortingOrder_(0)
{
    gameObject.renderer_ = this;
}

void Renderer::SetSortingOrder(int sortingOrder)
{
    // TODO - Have the singleton Application be able to return the scene
    // TODO - that is currently active and then change the key of the renderer.

    sortingOrder_ = sortingOrder;
}