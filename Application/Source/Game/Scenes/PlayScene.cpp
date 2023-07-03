#include "PlayScene.h"
#include "EC/Components/SpriteRenderer.h"

PlayScene::PlayScene()
    : Scene("PlayScene")
{
    nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");

    auto gameObject = CreateGameObject();
    auto spriteRenderer = gameObject->AddComponent<SpriteRenderer>(); 
    spriteRenderer->SetTexture(nareLogoTexture_);

}