#include "PhysicsWorld.h"

#include "Utils/Util.h"
#include "EC/Components/Collider2D.h"
#include "Physics/Bounds.h"
#include "EC/GameObject.h"

#include "Core/Application.h"

#include "Renderer/OrthographicCamera.h"

#include "Scene/SceneManager.h"

#include <algorithm>
#include "Core/Log.h"

#include <glm/gtx/string_cast.hpp>

uint32_t PhysicsWorld::AddCollider(Collider2D *collider)
{
    colliders_.emplace_back(collider);
    return colliders_.size() - 1;
}

void PhysicsWorld::RemoveColliderAtIndex(uint32_t index)
{
    Utility::RemoveAt(colliders_, index);
}

void PhysicsWorld::RemoveCollider(Collider2D *collider)
{
    colliders_.erase(std::remove(
        colliders_.begin(), colliders_.end(), collider), colliders_.end());
}

void PhysicsWorld::Update(Timestep ts)
{
    
}

void PhysicsWorld::FixedUpdate(float fixedDeltaTime)
{
    for (uint32_t i = 0; i < colliders_.size(); ++i)
    {
        Collider2D* colliderX = colliders_[i];

        if (!colliderX->enabled || !colliderX->GetGameObject().ActiveSelf())
        {
            continue;
        }

        for (uint32_t j = i + 1; j < colliders_.size(); ++j)
        {
            Collider2D* colliderY = colliders_[j];

            if (!colliderY->enabled || !colliderY->GetGameObject().ActiveSelf())
            {
                continue;
            }

            if (colliderX->GetBounds().Intersects(colliderY->GetBounds()))
            {
                colliderX->GetGameObject().OnTriggerEnter2D(colliderY);
                colliderY->GetGameObject().OnTriggerEnter2D(colliderX);
            }
        }
    }
}

std::vector<Collider2D *> PhysicsWorld::RaycastScreen(glm::vec2 mousePosition, bool includeInactive)
{
    std::vector<Collider2D*> colliders;
    auto camera = SceneManager::Instance()->GetActiveScene()->GetCamera();
    if (!camera)
    {
        return colliders;
    }

    float halfScreenWidth = static_cast<float>(Application::Instance()->GetWindowWidth()) *
        0.5f;
    float halfScreenHeight = static_cast<float>(Application::Instance()->GetWindowHeight()) *
        0.5f;
    glm::vec4 worldSpaceMouse = glm::inverse(camera->GetViewProjectionMatrix()) * 
        glm::vec4((mousePosition.x - halfScreenWidth) / halfScreenWidth, 
        (mousePosition.x - halfScreenHeight) / halfScreenHeight, -1, 1);

    // CC_TRACE("Mouse position: " + glm::to_string(mousePosition));
    // CC_TRACE(glm::to_string(worldSpaceMouse));
    for (Collider2D* collider : colliders_)
    {
        if (includeInactive)
        {
            if (collider->GetBounds().IsPointInside(worldSpaceMouse))
            {
                colliders.push_back(collider);
            }
        }
        else if (collider->enabled && collider->GetGameObject().ActiveSelf() && 
            collider->GetBounds().IsPointInside(worldSpaceMouse))
        {
            colliders.push_back(collider);
        }
    }
    return colliders;
}

std::vector<Collider2D *> PhysicsWorld::CircleCast(glm::vec3 position, float radius, bool includeInactive)
{
    std::vector<Collider2D*> colliders;
    for (Collider2D* collider : colliders_)
    {
        if (includeInactive)
        {
            if (glm::distance(collider->GetBounds().GetCenter(), position) <= radius)
            {
                colliders.push_back(collider);
            }
        }
        else if (collider->enabled && collider->GetGameObject().ActiveSelf() && 
            glm::distance(collider->GetBounds().GetCenter(), position) <= radius)
        {
            colliders.push_back(collider);
        }
    }
    return colliders;
}
