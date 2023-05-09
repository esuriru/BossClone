#include <vector>
#include <glm/glm.hpp>

#include "ECS/Entity.h"

class Quadtree
{
public:
    float gridAreaWidth = 16;
    float gridAreaHeight = 16;

private:
    std::vector<glm::vec2> areas_;
    std::vector<Entity> entitiesInAreas_;
        
};