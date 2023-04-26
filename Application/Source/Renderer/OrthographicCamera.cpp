#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

auto OrthographicCamera::RecalculateViewMatrix() -> void
{
    constexpr glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 transform = glm::translate(identity, position_) * 
        glm::rotate(identity, glm::radians(rotationDegrees_), glm::vec3(0, 0, 1));

    view_ = glm::inverse(transform);
    viewProjection_ = projection_ * view_;
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : projection_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
    , view_(1.0f)
    , position_(0.0f)
{
    RecalculateViewMatrix();
}

auto OrthographicCamera::SetProjection(float left, float right, float bottom, float top) -> void
{
    projection_ = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}
