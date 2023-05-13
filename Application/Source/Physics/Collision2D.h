#include "ECS/Entity.h"
#include <glm/glm.hpp>

struct Collision2D
{
    Entity OtherEntity;

    glm::vec2 Overlap;

    glm::vec2 Velocity, OtherVelocity;

    glm::vec2 Position, OtherPosition;
    glm::vec2 OldPosition, OldOtherPosition;

    Collision2D(Entity otherEntity, const glm::vec2& overlap = glm::vec2(), const glm::vec2& velocity = glm::vec2(),
        const glm::vec2& otherVelocity = glm::vec2(), const glm::vec2& position = glm::vec2(), const glm::vec2& otherPosition = glm::vec2(),
            const glm::vec2& oldPosition = glm::vec2(), const glm::vec2& oldOtherPosition = glm::vec2())
        : OtherEntity(otherEntity)
        , Overlap(overlap)
        , Velocity(velocity)
        , OtherVelocity(otherVelocity)
        , Position(position)
        , OtherPosition(otherPosition)
        , OldPosition(oldPosition)
        , OldOtherPosition(oldOtherPosition)
    {}

};