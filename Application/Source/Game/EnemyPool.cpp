#include "EnemyPool.h"

EnemyPool::EnemyPool(std::function<Ref<EnemyController>()> createFunc)
{
    _createFunc = createFunc;
}

void EnemyPool::Add(Ref<EnemyController> enemy)
{
    // enemy->SetPool(shared_from_this());
}

Ref<EnemyController> EnemyPool::Get()
{
    if (_enemies.empty())
    {
        auto c = _createFunc(); 
        // c->SetPool(shared_from_this());
        return c;
    }
    else
    {
        auto enemy = _enemies.back();
        _enemies.pop_back();
        enemy->Reset();
        return enemy;
    }

}

void EnemyPool::Release(Ref<EnemyController> enemy)
{
    _enemies.push_back(enemy);
}
