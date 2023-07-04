#pragma once

#include "Renderer.h"
#include "Tilemap.h"

class TilemapRenderer : public Renderer 
{
public:
    TilemapRenderer(GameObject& gameObject);
    
    inline void SetTilemapComponent(Tilemap* tilemapComponent)
    {
        tilemapComponent_ = tilemapComponent;
    }
    
    void Render() override;

private:
    Tilemap* tilemapComponent_;

};

