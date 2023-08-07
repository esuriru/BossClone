#include "ECS/System.h" 
#include "Core/Timestep.h"
#include "Core/Window.h"

class BatSystem : public System
{
public:
    BatSystem();
    void Update(Timestep ts);

    EventCallback eventCallback;
private:
    bool hasEnabledAnimation_ = false;
};