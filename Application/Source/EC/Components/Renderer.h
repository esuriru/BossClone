#pragma once
#include "EC/Component.h"

#include <memory>

class Renderer : public Component, public std::enable_shared_from_this<Renderer>
{
public:
    Renderer(Ref<GameObject> gameObject);

    void SetSortingOrder(int sortingOrder);
    inline int GetSortingOrder() const
    {
        return sortingOrder_;
    }

    virtual void Render() = 0;

private:
    int sortingOrder_;
};