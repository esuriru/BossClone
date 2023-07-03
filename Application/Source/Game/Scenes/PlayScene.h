#pragma once
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "Renderer/Texture.h"

class PlayScene : public Scene
{
public:
    PlayScene();

    inline virtual std::string GetName() override
    {
        return "PlayScene";
    }

private:
    Ref<Texture2D> nareLogoTexture_;

};
