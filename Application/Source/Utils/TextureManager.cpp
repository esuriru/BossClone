#include "TextureManager.h"

void TextureManager::RegisterTexture(Texture* t)
{
	currentLoadedTextures_.emplace_back(t);
}

void TextureManager::ClearTextures()
{
	for (auto& i : currentLoadedTextures_)
	{
		delete i;
	}
	currentLoadedTextures_.clear();
}
