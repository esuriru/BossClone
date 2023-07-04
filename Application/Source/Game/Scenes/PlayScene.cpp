#include "PlayScene.h"
#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/Tilemap.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

PlayScene::PlayScene()
    : Scene("PlayScene")
{
    nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");

    CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(100, 100, 1.f))
        ->SetComponent<SpriteRenderer>(nareLogoTexture_);
    
    CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f))
        ->SetComponent<Tilemap>();

    // auto spriteRenderer = gameObject->AddComponent<SpriteRenderer>(); 
    // spriteRenderer->SetTexture(nareLogoTexture_);
    // spriteRenderer->GetTransform().SetScale(glm::vec3(10.f));

}