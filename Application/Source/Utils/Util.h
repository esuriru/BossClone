#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <algorithm>
#include <vector>

#include <examples/imgui_impl_opengl3.h>

namespace Utility
{
    // TODO - Add custom UV coordinates
    using Vector2ui = std::pair<size_t, size_t>;

    glm::vec4 constexpr Colour32BitConvert(const glm::vec4& colour)
    {
        return glm::vec4(colour.r / 255.f, colour.g / 255.f, colour.b / 255.f, colour.a);
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

}
