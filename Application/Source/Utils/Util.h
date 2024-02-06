#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <algorithm>
#include <vector>


#include "Core/Core.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"
#include "Game/EightWayDirectionFlags.h"

namespace Utility
{
    void ImGuiImage(const Ref<Texture2D>& icon, float imageSizeMultiplier = 1.5f);
    void ImGuiImage(const Ref<Texture2D>& icon, float width, float height);
    void ImGuiImage(const Ref<SubTexture2D> &icon, float imageSizeMultiplier = 1.5f);

    // TODO - Add custom UV coordinates
    using Vector2ui = std::pair<size_t, size_t>;

    glm::vec4 constexpr Colour32BitConvert(const glm::vec4& colour)
    {
        return glm::vec4(colour.r / 255.f, colour.g / 255.f, colour.b / 255.f, colour.a);
    }

    constexpr uint32_t Hash(const char* s, int off = 0) noexcept
    {
        return !s[off] ? 5381 : (Hash(s, off+1)*33) ^ s[off];
    }

    template<typename T, typename A>
    void RemoveAt(std::vector<T, A>& container, size_t index)
    {
        container.erase(container.begin() + index);
    }

    template<typename C, typename T>
    bool Contains(C& container, T val)
    {
        return std::find(container.begin(), container.end(), val) != container.end();
    }

	inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
	{
		return a * (1.f - t) + b * t;
	}

	inline glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t)
	{
		return a * (1.f - t) + b * t;
	}

	inline glm::vec2 PerpendicularClockwise(const glm::vec2& lhs)
	{
		return glm::normalize(glm::vec2( lhs.y, -lhs.x));
	}

	inline glm::vec2 PerpendicularAntiClockwise(const glm::vec2& lhs)
	{
		return glm::normalize(glm::vec2( -lhs.y, lhs.x));
	}

    inline EightWayDirection DeltaToDirection(const glm::ivec2& delta)
    {
        if (delta.x == 0)
        {
            return delta.y < 0 ? 
                EightWayDirection::Up : EightWayDirection::Down;
        }
        else
        {
            return delta.x < 0 ? 
                EightWayDirection::Left : EightWayDirection::Right;
        }    
    }

    inline std::array<EightWayDirection, 3> GetOppositeAndAccessories(
        EightWayDirection direction)
    {
        switch (direction)
        {
            case EightWayDirection::Up:
                return
                {
                    {
                        EightWayDirection::Down,
                        EightWayDirection::Left,
                        EightWayDirection::Right
                    }
                };
            case EightWayDirection::Down:
                return
                {
                    {
                        EightWayDirection::Up,
                        EightWayDirection::Left,
                        EightWayDirection::Right
                    }
                };
            case EightWayDirection::Left:
                return
                {
                    {
                        EightWayDirection::Right,
                        EightWayDirection::Up,
                        EightWayDirection::Down
                    }
                };
            case EightWayDirection::Right:
                return
                {
                    {
                        EightWayDirection::Left,
                        EightWayDirection::Up,
                        EightWayDirection::Down
                    }
                };
            default:
                CC_ASSERT(false, "Direction not found");
                break;
        }
        return std::array<EightWayDirection, 3>();
    }

	inline float Cross(const glm::vec2& lhs, const glm::vec2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	double constexpr sqrtNewtonRaphson(double x, double curr, double prev)
    {
        return curr == prev
            ? curr
            : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
    }

	double constexpr sqrt(double x)
	{
        return x >= 0 && x < std::numeric_limits<double>::infinity()
            ? sqrtNewtonRaphson(x, x, 0)
            : std::numeric_limits<double>::quiet_NaN();
	}

    inline glm::ivec2 ConvertDirection(EightWayDirection direction)
    {
        switch (direction)
        {
            case EightWayDirection::Up:
                return { 0, 1 };
            case EightWayDirection::Up_Right:
                return { 1, 1 };
            case EightWayDirection::Right:
                return { 1, 0 };
            case EightWayDirection::Down_Right:
                return { 1, -1 };
            case EightWayDirection::Down:
                return { 0, -1 };
            case EightWayDirection::Down_Left:
                return { -1, -1 };
            case EightWayDirection::Left:
                return { -1, 0 };
            case EightWayDirection::Up_Left:
                return { -1, 1 };
            default:
                CC_FATAL("Direction could not be converted");
                break;
        }
        return { 0, 0 };
    }

}
