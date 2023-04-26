#pragma once
#include <glm/glm.hpp>

class OrthographicCamera
{
private:
    auto RecalculateViewMatrix() -> void;

public:
    OrthographicCamera(float left, float right, float bottom, float top);

    auto SetProjection(float left, float right, float bottom, float top) -> void;

    inline auto GetPosition() const -> const glm::vec3& { return position_; }
    inline auto SetPosition(const glm::vec3& pos) -> void
    { 
        position_ = pos;
        RecalculateViewMatrix();
    };

    inline auto GetRotation() const -> float { return rotationDegrees_; }
    inline auto SetRotation(float rot) -> void
    {
        rotationDegrees_ = rot;
        RecalculateViewMatrix();
    }


    inline auto GetProjectionMatrix() const -> const glm::mat4& { return projection_; }
    inline auto GetViewMatrix() const -> const glm::mat4& { return view_; }
    inline auto GetViewProjectionMatrix() const -> const glm::mat4& { return viewProjection_; }

private:
    // Matrices
    glm::mat4 projection_, view_, viewProjection_;

    // NOTE - An ortho cam can realistically only rotate around the z-axis. Hence, only float is needed.
    float rotationDegrees_;

    glm::vec3 position_;

};