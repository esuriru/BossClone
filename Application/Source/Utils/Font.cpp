#include "Font.h"
#include <stb_image.h>
#include "Core/Core.h"
#include <fstream>
#include <iostream>

Font* Font::LoadFont(const std::string& font_path, const std::string& spacing_csv_path)
{
	// Font manages it's own texture
	int width, height, channels;

	// Flip the image
	stbi_set_flip_vertically_on_load(1);
	const auto& data = stbi_load(font_path.data(), &width, &height, &channels, 0);

	if (!data)
	{
		CC_ERROR("Failed to load image! Path: ", std::string(font_path));
		return nullptr;
	}

	Texture* t = new Texture();

	t->path_ = std::string(font_path);
	t->width_ = width;
	t->height_ = height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	t->internalFormat_ = internalFormat;
	t->dataFormat_ = dataFormat;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glCreateTextures(GL_TEXTURE_2D, 1, &t->id_);
	glTextureStorage2D(t->id_, 1, internalFormat, static_cast<GLint>(t->width_), static_cast<GLint>(t->height_));

	glTextureParameteri(t->id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(t->id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTextureParameteri(t->id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(t->id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(t->id_, 0, 0, 0, static_cast<GLint>(t->width_), static_cast<GLint>(t->height_), dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	std::ifstream fileStream(spacing_csv_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << font_path << ". Are you in the right directory ?\n";
		return nullptr;
	}

	std::array<int, NO_OF_CHARACTERS> char_data{};

	std::string line;
	while (std::getline(fileStream, line))
	{
		if (line.substr(0, 5) == "Char ")
		{
			int character = 0;
			int width = 0;
			int matches = sscanf_s(line.data(), "Char %d Base Width,%d", &character, &width);

			if (matches == 2)
				char_data[character] = width;
		}
	}

	Font* font = new Font(char_data);
	font->fontTexture = t;
	font->fontMultiplier = 0.1f;
	return font;
}

Font::~Font()
{
	delete fontTexture;
}

Font::Font(std::array<int, NO_OF_CHARACTERS> data)
	: fontSpacing(data)
{
}
