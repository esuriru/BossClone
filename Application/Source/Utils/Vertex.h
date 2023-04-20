#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>


struct Vertex
{
	glm::vec3 pos{};
	glm::vec4 color = { 0,0,0,1 };
	glm::vec3 normal{};
	glm::vec2 texCoord{};
	glm::vec3 tangent{};
	glm::vec3 bitangent{};
	Vertex() {}
};

#endif