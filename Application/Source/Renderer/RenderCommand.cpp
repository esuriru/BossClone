#include "RenderCommand.h"
#include <glad/glad.h>

auto RenderCommand::Init() -> void
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
}

auto RenderCommand::SetClearColour(const glm::vec4 &colour) -> void
{
    glClearColor(colour.r, colour.g, colour.b, colour.a);
}

auto RenderCommand::Clear() -> void
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

auto RenderCommand::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount) -> void
{
    // TODO - Might be better to make an overload of this function so that a if statement doesn't get run everytime we want to render something.
    // TODO - Wary about this vertex array bind.
    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, static_cast<GLint>(count), GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

auto RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    glViewport(x, y, width, height);
}
