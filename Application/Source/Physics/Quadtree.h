#include <vector>
#include <glm/glm.hpp>

#include "ECS/Entity.h"

class Quadtree
{
public:
    float gridAreaWidth = 16;
    float gridAreaHeight = 16;

private:
    std::vector<Entity> objectsInArea_; 
        
};