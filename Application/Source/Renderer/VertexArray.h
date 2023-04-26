#pragma once

#include "Core/Core.h"
#include "Buffer.h"

class VertexArray
{
public:
    VertexArray();
    virtual ~VertexArray();

    auto Bind() const -> void;
    auto Unbind() const -> void;

    auto AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) -> void;
    auto SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) -> void;

    inline virtual auto GetVertexBuffers() const -> const std::vector<Ref<VertexBuffer>>& 
    {
        return vertexBuffers_;
    }
    inline virtual auto GetIndexBuffer() const -> const Ref<IndexBuffer>&
    {
        return indexBuffer_;
    }

private:
    std::vector<Ref<VertexBuffer>> vertexBuffers_;
    Ref<IndexBuffer> indexBuffer_;

    uint32_t rendererID_;
};

