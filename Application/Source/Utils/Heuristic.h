#include <glm/glm.hpp>

namespace Heuristic
{
    inline unsigned int Manhattan(const glm::vec2& v1, const glm::vec2& v2,
        int weight)
    {
        glm::vec2 delta = v2 - v1;
        return static_cast<unsigned int>(weight * (delta.x + delta.y));
    }

    inline unsigned int Euclidean(const glm::vec2& v1, const glm::vec2& v2,
        int weight)
    {
        glm::vec2 delta = v2 - v1;
        return static_cast<unsigned int>(weight * sqrt(
            (delta.x * delta.x) + (delta.y * delta.y)
        ));
    }
};