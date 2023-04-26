#include "Projection.h"
#include "MyMath.h"

float Projection::GetOverlap(const Projection& other)
{
	if (Overlap(other))
	{
		return Math::Min(max, other.max) - Math::Max(min, other.min);
	}
	return 0;
}

bool Projection::Overlap(const Projection& other)
{
	return !(min > other.max || other.min > max);
}
