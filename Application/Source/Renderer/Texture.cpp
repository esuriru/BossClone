#include "Texture.h"
#include "Utils/TextureManager.h"
#include "Utils/LoadTGA.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "Core/Log.h"


Texture::Texture(const char* path, bool tga_file)
{
	TextureManager::GetInstance()->RegisterTexture(this);
	path_ = std::string(path);
	if (tga_file)
		id_ = LoadTGA(path);
	else
	{
		// Create buffers
		int width, height, channels;

		// Flip the image
		stbi_set_flip_vertically_on_load(1);
		const auto& data = stbi_load(path, &width, &height, &channels, 0);

		if (!data)
		{
			CC_ERROR("Failed to load image! Path: ", path_);
		}

		width_ = width;
		height_ = height;

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

        internalFormat_ = internalFormat;
        dataFormat_ = dataFormat;

		//if (internalFormat & dataFormat)
		//{
		//	std::cout << "Format not supported\n";
		//}

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		glCreateTextures(GL_TEXTURE_2D, 1, &id_);
		glTextureStorage2D(id_, 1, internalFormat, static_cast<GLint>(width_), static_cast<GLint>(height_));

		glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(id_, 0, 0, 0, static_cast<GLint>(width_), static_cast<GLint>(height_), dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}

void Texture::Release()
{
	glDeleteTextures(1, &id_);
}

Texture::~Texture()
{
	Release();
}

Texture::Texture(Texture&& other)
	: id_(other.id_)
{
	other.id_ = 0;
}

Texture& Texture::operator=(Texture&& other)
{
	if (this != &other)
	{
		Release();
		std::swap(id_, other.id_);
	}
	return *this;
}

void Texture::Bind(uint32_t slot)
{
	//glActiveTexture(slot);
	//glBindTexture(GL_TEXTURE_2D, id_);
	glBindTextureUnit(slot, id_);
}

void Texture::Unbind()
{
}

Texture* Texture::LoadTGATexture(const char* tga_file_path)
{
	return new Texture(tga_file_path, true);
}

Texture* Texture::LoadTexture(const char* file_path_for_stb, bool managed)
{
	int width, height, channels;

	// Flip the image
	stbi_set_flip_vertically_on_load(1);
	const auto& data = stbi_load(file_path_for_stb, &width, &height, &channels, 0);

	if (!data)
	{
		CC_ERROR("Failed to load image! Path: ", std::string(file_path_for_stb));
		return nullptr;
	}

	Texture* t = new Texture();
	if (managed)
		TextureManager::GetInstance()->RegisterTexture(t);

	t->path_ = std::string(file_path_for_stb);
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

	//if (internalFormat & dataFormat)
	//{
	//	std::cout << "Format not supported\n";
	//}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glCreateTextures(GL_TEXTURE_2D, 1, &t->id_);
	glTextureStorage2D(t->id_, 1, internalFormat, static_cast<GLint>(t->width_), static_cast<GLint>(t->height_));

	glTextureParameteri(t->id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(t->id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTextureParameteri(t->id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(t->id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(t->id_, 0, 0, 0, static_cast<GLint>(t->width_), static_cast<GLint>(t->height_), dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	
	return t;
}

Texture* Texture::LoadCubemap(const std::vector<std::string>& file_paths)
{
	Texture* temp = new Texture();
	temp->id_ = LoadTGACubemap(file_paths);
	return temp;
}

