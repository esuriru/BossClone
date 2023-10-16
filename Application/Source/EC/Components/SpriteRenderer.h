#pragma once

#include "Renderer.h"
#include "EC/Components/Transform.h"
#include "Renderer/SubTexture2D.h"

class SpriteRenderer : public Renderer
{
public:
    SpriteRenderer(GameObject& gameObject);
    SpriteRenderer(GameObject& gameObject, Ref<SubTexture2D> subtexture);
    SpriteRenderer(GameObject& gameObject, Ref<Texture2D> subtexture);

    void Render() override; 

    void SetTexture(Ref<SubTexture2D> subtexture);
    void SetTexture(Ref<Texture2D> texture);

    void SetNativeSize();

    glm::vec4 colour;
    float tilingFactor;

private:
    Ref<SubTexture2D> subtexture_;
};