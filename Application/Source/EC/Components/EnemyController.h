#pragma once

#include "EC/Component.h"

class EnemyController : public Component
{
public:
    EnemyController(GameObject& gameObject);

    void Start() override;
private:

};