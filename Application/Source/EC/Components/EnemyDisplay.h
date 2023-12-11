#pragma once

#include "EC/Component.h"
class EnemyController;

class EnemyDisplay : public Component
{
public:
    EnemyDisplay(GameObject& gameObject);

    void Update(Timestep ts) override;
    void OnImGuiRender() override;

private:
    void TextCentred(std::string text);

    Ref<EnemyController> lastClickedEnemy_;
    std::string enemyName_;
    std::string enemyCurrentHealth_;
    std::string enemyCurrentState_;
};