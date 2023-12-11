#pragma once

#include <vector>
#include <functional>

#include "Core/Core.h"

#include "EC/Components/EnemyController.h"


class EnemyPool : public std::enable_shared_from_this<EnemyPool>
{
private:
    std::vector<Ref<EnemyController>> _enemies;
    std::function<Ref<EnemyController>()> _createFunc;

public:
    EnemyPool(std::function<Ref<EnemyController>()> createFunc);

    void Add(Ref<EnemyController> enemy);
    Ref<EnemyController> Get();
    void Release(Ref<EnemyController> enemy);
};