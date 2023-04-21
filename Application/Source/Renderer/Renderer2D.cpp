#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// const auto& projection = Matrix4x4::Ortho(-1.6f, 1.6f, -0.9f, 0.9f, -10, 10);
// const auto& view = Matrix4x4::Translate({0, 0, 0}).Inverse();
struct QuadVertex
{
    glm::vec3 position;
    glm::vec4 colour;
    glm::vec2 texCoord;
    float texIndex;
    float tilingFactor;
};

struct Renderer2DData
{
    static const uint32_t MaxQuads = 10000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32; // TODO - Render capabilities

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;

    Ref<Shader> TextureShader;
    Ref<Texture> WhiteTexture;

    uint32_t QuadIndexCount = 0;

    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1; // 0 = white texture

    glm::vec4 QuadVertexPositions[4]; 

    glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

    // TODO - Make a macro that makes it so that stats aren't calculated.
    Renderer2D::Stats stats;
};


static Renderer2DData s_data;

auto Renderer2D::Init() -> void
{
    s_data.QuadVertexArray = CreateRef<VertexArray>(); 

    s_data.QuadVertexBuffer = CreateRef<VertexBuffer>(s_data.MaxVertices * sizeof(QuadVertex));

    s_data.QuadVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "vertexPosition" },
        { ShaderDataType::Float4, "vertexColour" },
        { ShaderDataType::Float2, "vertexTexCoords" },
        { ShaderDataType::Float, "textureIndex" },
        { ShaderDataType::Float, "tilingFactor" },
    });

    s_data.QuadVertexArray->AddVertexBuffer(s_data.QuadVertexBuffer);

    s_data.QuadVertexBufferBase = new QuadVertex[s_data.MaxVertices];

    uint32_t* quadIndices = new uint32_t[s_data.MaxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_data.MaxIndices; i+=6)
    {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }
    Ref<IndexBuffer> squareIB = CreateRef<IndexBuffer>(quadIndices, s_data.MaxIndices);

    s_data.QuadVertexArray->SetIndexBuffer(squareIB);
    delete[] quadIndices;


    s_data.WhiteTexture = CreateRef<Texture2D>(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    s_data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    int32_t samplers[s_data.MaxTextureSlots];
    for (int32_t i = 0; i < s_data.MaxTextureSlots; ++i)
        samplers[i] = i;

    s_data.TextureShader = CreateRef<Shader>("Shader/Texture.glsl");
    s_data.TextureShader->Bind();
    s_data.TextureShader->SetIntArray("u_Textures", samplers, s_data.MaxTextureSlots);

    s_data.TextureSlots[0] = s_data.WhiteTexture;

    s_data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0, 1.0f };
    s_data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0, 1.0f };
    s_data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0, 1.0f };
    s_data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0, 1.0f };
}

auto Renderer2D::Exit() -> void
{
    delete[] s_data.QuadVertexBufferBase;
}

auto Renderer2D::BeginScene(const OrthographicCamera& camera) -> void
{
    s_data.TextureShader->Bind();

    s_data.TextureShader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());

    StartBatch();
}

auto Renderer2D::EndScene() -> void
{
    uint32_t dataSize = static_cast<uint32_t>((uint8_t*)s_data.QuadVertexBufferPtr - (uint8_t*)s_data.QuadVertexBufferBase);
    s_data.QuadVertexBuffer->SetData(s_data.QuadVertexBufferBase, dataSize);

    Flush();
}

auto Renderer2D::Flush() -> void
{
    for (uint32_t i = 0; i < s_data.TextureSlotIndex; ++i)
        s_data.TextureSlots[i]->Bind(i);

    RenderCommand::DrawIndexed(s_data.QuadVertexArray, s_data.QuadIndexCount);
    ++s_data.stats.DrawCalls;
}

auto Renderer2D::GetStats() -> Renderer2D::Stats
{
    return s_data.stats;
}

auto Renderer2D::ResetStats() -> void
{
    memset(&s_data.stats, 0, sizeof(Stats));
}

auto Renderer2D::FlushAndReset() -> void
{
    EndScene();

    s_data.QuadIndexCount = 0;
    s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

    s_data.TextureSlotIndex = 1;
}

auto Renderer2D::StartBatch() -> void
{
    s_data.QuadIndexCount = 0;
    s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

    s_data.TextureSlotIndex = 1;
}

auto Renderer2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tilingFactor) -> void
{
    // TODO - make this a static colour, or even make a helper class that stores a glm::vec4.
    constexpr glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr size_t quadVertexCount = 4;
    const glm::vec2* texCoords = subtexture->GetTexCoords();
    const Ref<Texture2D> texture = subtexture->GetTexture();

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.TextureSlotIndex; ++i)
    {
        if (*s_data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = static_cast<float>(s_data.TextureSlotIndex);
        s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
        ++s_data.TextureSlotIndex;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = white;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = textureIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++(s_data.QuadVertexBufferPtr);
    }
    s_data.QuadIndexCount += 6;
    ++s_data.stats.QuadCount;

}

auto Renderer2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &colour) -> void
{
    constexpr size_t quadVertexCount = 4;
    constexpr float texIndex = 0.0f; // White texture
    constexpr float tilingFactor = 1.0f; 
    constexpr std::array<glm::vec2, 4> texCoords {
        {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    };

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = colour;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = texIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++s_data.QuadVertexBufferPtr;
    }

    s_data.QuadIndexCount += 6;
    ++s_data.stats.QuadCount;
}


auto Renderer2D::DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotationDegrees, const glm::vec4 &colour) -> void
{
    constexpr size_t quadVertexCount = 4;
    constexpr float texIndex = 0.0f; // White texture
    constexpr float tilingFactor = 1.0f; 
    constexpr std::array<glm::vec2, 4> texCoords {
        {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    };

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = colour;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = texIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++(s_data.QuadVertexBufferPtr);
    }

    s_data.QuadIndexCount += 6;
    ++s_data.stats.QuadCount;
}

auto Renderer2D::DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotationDegrees, const Ref<Texture2D> &texture, float tilingFactor) -> void
{
    // TODO - make this a static colour, or even make a helper class that stores a glm::vec4.
    constexpr glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr size_t quadVertexCount = 4;
    constexpr std::array<glm::vec2, 4> texCoords {
        {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    };

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.TextureSlotIndex; ++i)
    {
        if (*s_data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = static_cast<float>(s_data.TextureSlotIndex);
        s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
        ++s_data.TextureSlotIndex;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = white;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = textureIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++(s_data.QuadVertexBufferPtr);
    }

    s_data.QuadIndexCount += 6;
    ++s_data.stats.QuadCount;
}

auto Renderer2D::DrawRotatedQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotationDegrees, const Ref<SubTexture2D> &subtexture, float tilingFactor) -> void
{
    // TODO - make this a static colour, or even make a helper class that stores a glm::vec4.
    constexpr glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr size_t quadVertexCount = 4;
    const glm::vec2* texCoords = subtexture->GetTexCoords();
    const Ref<Texture2D> texture = subtexture->GetTexture();

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.TextureSlotIndex; ++i)
    {
        if (*s_data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = static_cast<float>(s_data.TextureSlotIndex);
        s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
        ++s_data.TextureSlotIndex;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = white;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = textureIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++(s_data.QuadVertexBufferPtr);
    }
    s_data.QuadIndexCount += 6;

    ++s_data.stats.QuadCount;
}


auto Renderer2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor) -> void
{
    // TODO - make this a static colour, or even make a helper class that stores a glm::vec4.
    constexpr glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr size_t quadVertexCount = 4;
    constexpr std::array<glm::vec2, 4> texCoords {
        {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    };

    if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
        FlushAndReset();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.TextureSlotIndex; ++i)
    {
        if (*s_data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        if (s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
            FlushAndReset();
        textureIndex = static_cast<float>(s_data.TextureSlotIndex);
        s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
        ++s_data.TextureSlotIndex;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    for (size_t i = 0; i < quadVertexCount; ++i)
    {
        s_data.QuadVertexBufferPtr->position = model * s_data.QuadVertexPositions[i];
        s_data.QuadVertexBufferPtr->colour = white;
        s_data.QuadVertexBufferPtr->texCoord = texCoords[i];
        s_data.QuadVertexBufferPtr->texIndex = textureIndex;
        s_data.QuadVertexBufferPtr->tilingFactor = tilingFactor;
        ++(s_data.QuadVertexBufferPtr);
    }

    s_data.QuadIndexCount += 6;
    ++s_data.stats.QuadCount;

}