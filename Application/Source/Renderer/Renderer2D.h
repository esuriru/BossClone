#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include "SubTexture2D.h"
#include "OrthographicCamera.h"

/// @brief Batch renderer static class
class Renderer2D
{
public:
    static auto Init() -> void;
    static auto Exit() -> void;

    static auto BeginScene(const OrthographicCamera& camera) -> void;
    static auto EndScene() -> void;

    static auto Flush() -> void;

#pragma region QUAD
    // Flat colours
    static auto DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &colour) -> void;
    static auto DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &colour) -> void;

    // Textures
    static auto DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor = 1.0f) -> void;
    static auto DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tilingFactor = 1.0f) -> void;
    static auto DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tilingFactor = 1.0f) -> void;
    static auto DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<SubTexture2D> &subtexture, float tilingFactor = 1.0f) -> void;
#pragma endregion QUAD

    struct Stats
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        inline uint32_t GetTotalVertexCount()
        {
            return QuadCount * 4;
        }

        inline uint32_t GetTotalIndexCount()
        {
            return QuadCount * 6;
        }
    };
    static auto GetStats() -> Stats;
    static auto ResetStats() -> void;

private:
    static auto FlushAndReset() -> void;
    static auto StartBatch() -> void;
};
