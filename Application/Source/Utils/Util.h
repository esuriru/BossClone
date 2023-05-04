#pragma once
#include <glm/glm.hpp>

namespace Utility
{
    template<typename T1, typename T2>
    bool Contains(T1 container, T2 val)
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
