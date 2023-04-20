#pragma once
#include "Renderer/Texture.h"
#include <string>
#include <array>
#include <limits>

constexpr size_t NO_OF_CHARACTERS = 256;

struct Font
{
	static Font* LoadFont(const std::string& font_path, const std::string& spacing_csv_path);

	Texture* fontTexture = nullptr;
	const std::array<int, NO_OF_CHARACTERS> fontSpacing;
	float fontMultiplier = 0.1f;

	~Font();
	Font(std::array<int, NO_OF_CHARACTERS> data);
};

