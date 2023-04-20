#include "TerrainVolume.h"
#include "BoundingSphere.h"
#include <stdexcept>
#include "AABB.h"
#include <glm/gtc/constants.hpp>

#include "Terrain.h"

namespace Utility
{
	inline float barry_centric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2 pos)
	{
		// https://johnshatterly.wordpress.com/2017/08/09/graphics-programming-simple-terrain-collision-using-barycentric-coordinates/
		//float det = ((p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z));
		//det = std::max(det, 0.0001f);

		//std::cout << det << "\n";

		//float id = 1.f / det;
		//float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) * id;
		//float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) * id;
		//float l3 = 1.0f - l1 - l2;
		//const auto& result = l1 * p1.y + l2 * p2.y + l3 * p3.y;
		//return result;



		float b1 = (p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z);
		float b2 = (p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z);
		float b3 = 1.0f - b1 - b2;

		return (b1 * p1.y + b2 * p2.y + b3 * p3.y);
#if 0
		if (isnan(result))
			throw std::logic_error("Not a number returned");
#endif
	}
}

TerrainVolume::TerrainVolume(Collider* parent)
	: BoundingVolume(parent)
{

}

bool TerrainVolume::IsIntersecting(const TerrainVolume* vol) const
{
	throw std::logic_error("Function not implemented");
}

bool TerrainVolume::IsIntersecting(const AABB* other) const
{
	return other->IsIntersecting(this);
}

bool TerrainVolume::IsIntersecting(const BoundingSphere* other) const
{
	return other->IsIntersecting(this);
}

bool TerrainVolume::IsIntersecting(const RayVolume* other) const
{
	return false;
}

std::pair<float, glm::vec3> TerrainVolume::GetHeightAndNormal(float x, float z) const
{
	float terrainX = x - (centre_.x);
	float terrainZ = z - (centre_.z);
	//float terrainX = x;
	//float terrainZ = z;

	float gridSquareSize = TERRAIN_SIZE * inverseHeightsRowMinusOne;
	float inverseGridSquareSize = 1 / gridSquareSize;

	int gridX = static_cast<int>(std::floor(terrainX * inverseGridSquareSize));
	int gridZ = static_cast<int>(std::floor(terrainZ * inverseGridSquareSize));

	if (gridX >= heights_.size() - 1 || gridZ >= heights_.size() - 1 || gridX < 0 || gridZ < 0)
	{
		return { 0, glm::vec3()};
	}

	float xCoord = (fmod(terrainX, gridSquareSize)) * inverseGridSquareSize;
	float zCoord = (fmod(terrainZ, gridSquareSize)) * inverseGridSquareSize;

	//std::cout << _heights[gridX][gridZ] << "\n";
	glm::vec3 normal;
	constexpr float ONE_THIRD = 1.f / 3.f;
	normal = ((xCoord <= 1 - zCoord) ? (normals_[gridX][gridZ] + normals_[gridX + 1][gridZ] + normals_[gridX][gridZ + 1])
		: (normals_[gridX + 1][gridZ] + normals_[gridX + 1][gridZ + 1] + normals_[gridX][gridZ + 1]) ) * ONE_THIRD;

	glm::normalize(normal);
	
	return { centre_.y + ((xCoord <= 1 - zCoord) ?
		Utility::barry_centric(
			{ 0, heights_[gridX][gridZ], 0 },
			{ 1, heights_[gridX + 1][gridZ], 0 },
			{ 0, heights_[gridX][gridZ + 1], 1 },
			{ xCoord, zCoord }
		) :
		Utility::barry_centric(
			{ 1, heights_[gridX + 1][gridZ], 0 },
			{ 1, heights_[gridX + 1][gridZ + 1], 0 },
			{ 0, heights_[gridX][gridZ + 1], 1 },
			{ xCoord, zCoord }
	)), /*_normals[gridX][gridZ]*/ normal };
}

float TerrainVolume::GetHeight(float x, float z) const
{
	float terrainX = x - (centre_.x);
	float terrainZ = z - (centre_.z);
	//float terrainX = x;
	//float terrainZ = z;

	float gridSquareSize = TERRAIN_SIZE * inverseHeightsRowMinusOne;
	float inverseGridSquareSize = 1 / gridSquareSize;

	int gridX = static_cast<int>(std::floor(terrainX * inverseGridSquareSize));
	int gridZ = static_cast<int>(std::floor(terrainZ * inverseGridSquareSize));

	if (gridX >= heights_.size() - 1 || gridZ >= heights_.size() - 1 || gridX < 0 || gridZ < 0)
	{
		return 0;
	}

	float xCoord = (fmod(terrainX, gridSquareSize)) * inverseGridSquareSize;
	float zCoord = (fmod(terrainZ, gridSquareSize)) * inverseGridSquareSize;
	
	return centre_.y + ((xCoord <= 1 - zCoord) ?
		Utility::barry_centric(
			{ 0, heights_[gridX][gridZ], 0 },
			{ 1, heights_[gridX + 1][gridZ], 0 },
			{ 0, heights_[gridX][gridZ + 1], 1 },
			{ xCoord, zCoord }
		) :
		Utility::barry_centric(
			{ 1, heights_[gridX + 1][gridZ], 0 },
			{ 1, heights_[gridX + 1][gridZ + 1], 0 },
			{ 0, heights_[gridX][gridZ + 1], 1 },
			{ xCoord, zCoord }
		));
}
