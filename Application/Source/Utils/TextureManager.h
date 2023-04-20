#pragma once
#include "Singleton.h"
#include <vector>
#include "Renderer/Texture.h"

class TextureManager : public Singleton<TextureManager>
{
public:
	TextureManager() = default;
	~TextureManager() = default;

	void RegisterTexture(Texture* t);
	void ClearTextures();

private:
	std::vector<Texture*> currentLoadedTextures_;
	
};
