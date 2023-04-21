#include "Buffer.h"

#include "Core/Core.h"
#include "glad/glad.h"

#pragma region VERTEX_BUFFER

VertexBuffer::VertexBuffer(float *vertices, uint32_t size)
{
    glCreateBuffers(1, &rendererID_);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(size), vertices, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(uint32_t size)
{
    glCreateBuffers(1, &rendererID_);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(size), nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &rendererID_);
}

auto VertexBuffer::Bind() const -> void
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID_);
}

auto VertexBuffer::Unbind() const -> void
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

auto VertexBuffer::SetData(const void *data, uint32_t size) -> void
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

#pragma endregion 

#pragma region INDEX_BUFFER

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
    : count_(count)
{
    glCreateBuffers(1, &rendererID_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(count * sizeof(uint32_t)), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &rendererID_);
}

auto IndexBuffer::Bind() const -> void
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID_);
}

auto IndexBuffer::Unbind() const -> void
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#pragma endregion


