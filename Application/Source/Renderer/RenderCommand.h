#pragma once
#include "VertexArray.h"
#include <glm/glm.hpp>
#include <cstdint>

class RenderCommand final
{
public:
    static auto Init() -> void;
    static auto SetClearColour(const glm::vec4& colour) -> void;
    static auto Clear() -> void;
    static auto DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) -> void;
    static auto SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;
};