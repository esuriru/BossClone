#pragma once
struct Projection
{
	float min;
	float max;

	float GetOverlap(const Projection& other);
	bool Overlap(const Projection& other);
};

