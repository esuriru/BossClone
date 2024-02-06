#include <glm/glm.hpp>

namespace Heuristic
{
    inline unsigned int Manhattan(const glm::ivec2& v1, const glm::ivec2& v2,
        int weight)
    {
        glm::ivec2 delta = v2 - v1;
        return static_cast<unsigned int>(weight * (delta.x + delta.y));
    }

    inline unsigned int Euclidean(const glm::ivec2& v1, const glm::ivec2& v2,
        int weight)
    {
        glm::ivec2 delta = v2 - v1;
        return static_cast<unsigned int>(weight * sqrt(
            (delta.x * delta.x) + (delta.y * delta.y)
        ));
    }
};