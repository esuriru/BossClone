#pragma once
#include "EC/Component.h"

#include <memory>

class Renderer : public Component
{
public:
    Renderer(GameObject& gameObject);

    void SetSortingOrder(int sortingOrder);
    inline int GetSortingOrder() const
    {
        return sortingOrder_;
    }

    virtual void Render() = 0;

private:
    int sortingOrder_;
};