#pragma once

#include <vector>
#include <Core/Core.h>
#include <cstdint>

class Shader;

enum class ShaderDataType
{
    None = 0, 
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
};

static constexpr auto ShaderDataTypeSize(ShaderDataType type) -> uint32_t
{
    constexpr uint32_t f_size = sizeof(float);
    switch (type)
    {
        case ShaderDataType::Float: return f_size;
        case ShaderDataType::Float2: return f_size * 2;
        case ShaderDataType::Float3: return f_size * 3;
        case ShaderDataType::Float4: return f_size * 4;

        case ShaderDataType::Mat3: return f_size * 3 * 3;
        case ShaderDataType::Mat4: return f_size * 4 * 4;

        // Int size = float size (4 bytes)
        case ShaderDataType::Int: return f_size;
        case ShaderDataType::Int2: return f_size * 2;
        case ShaderDataType::Int3: return f_size * 3;
        case ShaderDataType::Int4: return f_size * 4;

        // A bool is 1 byte
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::None: break;
    }

    CC_ASSERT(false, "Unknown ShaderDataType.");
    return 0;
}

struct BufferElement
{
    std::string Name;
    ShaderDataType Type;
    uint32_t Size;
    uint32_t Offset;
    bool Normalized;

    BufferElement() = default;

    BufferElement(ShaderDataType type, const std::string& name, const bool normalized = false)
        : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

    auto GetComponentCount() const -> uint32_t
    {
        switch (Type)
        {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;

            case ShaderDataType::Mat3: return 3 * 3;
            case ShaderDataType::Mat4: return 4 * 4;

            case ShaderDataType::Int: return 1;
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::Int4: return 4;

            case ShaderDataType::Bool: return 1;

            case ShaderDataType::None: break;
        }

        CC_ASSERT(false, "Unknown ShaderDataType.")
        return 0;
    }
};

/**
 * \brief Layout of the elements in a buffer. Should be constructed when initializing a scene.
 */
class BufferLayout
{
public:
    BufferLayout(std::initializer_list<BufferElement> elements)
        : elements_(elements)
        , stride_(0)
    {
        CalculateOffsetsAndStride();
    }


    BufferLayout() = default;

    inline auto GetElements() const -> const std::vector<BufferElement>&
    {
        return elements_;
    }

    inline auto GetStride() const -> uint32_t
    {
        return stride_;
    }

    auto begin() -> std::vector<BufferElement>::iterator { return elements_.begin(); }
    auto end() -> std::vector<BufferElement>::iterator { return elements_.end(); }

    auto begin() const -> std::vector<BufferElement>::const_iterator { return elements_.begin(); }
    auto end() const -> std::vector<BufferElement>::const_iterator { return elements_.end(); }


private:
    /**
     * \brief Cached list of elements. Will get dirtied if modified. Need to recalculate offsets and stride if done.
     */
    std::vector<BufferElement> elements_;
    uint32_t stride_;

    inline auto CalculateOffsetsAndStride() -> void
    {
        uint32_t offset = 0;
        stride_ = 0;
        for (auto& element : elements_)
        {
            element.Offset = offset;
            offset += element.Size;
            stride_ += element.Size;
        }
    }
};

class VertexBuffer
{
public:
    VertexBuffer(float* vertices, uint32_t size);
    VertexBuffer(uint32_t size);
    ~VertexBuffer();
    
    auto Bind() const -> void;
    auto Unbind() const -> void;

    inline auto GetLayout() const -> const BufferLayout&
    {
        return layout_;
    }
    inline auto SetLayout(const BufferLayout& layout) -> void
    {
        layout_ = layout;
    }

    virtual auto SetData(const void* data, uint32_t size) -> void;

private:
    uint32_t rendererID_;
    BufferLayout layout_;
};

class IndexBuffer
{
public:
    IndexBuffer(uint32_t* indices, uint32_t count);
    ~IndexBuffer();

    auto Bind() const -> void;
    auto Unbind() const -> void;

    inline auto GetCount() const -> uint32_t
    {
        return count_;
    }

private:
    uint32_t rendererID_;
    uint32_t count_;
};