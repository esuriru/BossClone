#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stb_image.h>
#include "Core/Core.h"

#include "LoadTGA.h"

GLuint LoadTGACubemap(const std::vector<std::string>& file_paths)
{

	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	for (int i = 0; i < file_paths.size(); ++i)
	{
		stbi_set_flip_vertically_on_load(0);

		const auto& data = stbi_load(file_paths[i].data(), &width, &height, &channels, 0);

		if (!data)
		{
			CC_ERROR("Failed to load image! Path: ", file_paths[i]);
			return 0;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		if (glGetError() == GL_INVALID_OPERATION || glGetError() == GL_INVALID_VALUE)
			CC_ERROR("Could not create a texture in the cubemap.");
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return id;
}

GLuint LoadTGA(const char* file_path)				// load TGA file to memory
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open()) {
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return 0;
	}

	GLubyte		header[18];									// first 6 useful header bytes
	GLuint		bytesPerPixel;								    // number of bytes per pixel in TGA gile
	GLuint		imageSize;									    // for setting memory
	GLubyte* data;
	GLuint		mainTexture = 0;
	unsigned	width, height;

	fileStream.read((char*)header, 18);
	width = header[12] + header[13] * 256;
	height = header[14] + header[15] * 256;

	if (width <= 0 ||								// is width <= 0
		height <= 0 ||								// is height <=0
		(header[16] != 24 && header[16] != 32))		// is TGA 24 or 32 Bit
	{
		fileStream.close();							// close file on failure
		std::cout << "File header error.\n";
		return 0;
	}


	bytesPerPixel	= header[16] / 8;						//divide by 8 to get bytes per pixel
	imageSize		= width * height * bytesPerPixel;	// calculate memory required for TGA data
	
	data = new GLubyte[ imageSize ];
	fileStream.seekg(18, std::ios::beg);
	fileStream.read((char *)data, imageSize);
	fileStream.close();	

	//int byte = (header[17] >> 5) & 1;
	int flip = (header[17] & 0x20);

	if (flip)
	{
		for (int i = 0; i < height / 2; ++i)
		{
			int h = height - i - 1;
			for (int j = 0; j < width * bytesPerPixel; ++j)
			{
				std::swap(data[i * width * bytesPerPixel + j], data[h * width * bytesPerPixel + j]);
			}
		}
	}

	glGenTextures(1, &mainTexture);
	glBindTexture(GL_TEXTURE_2D, mainTexture);
	if(bytesPerPixel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	else //bytesPerPixel == 4
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//float maxAnisotropy = 1.f;
	//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)maxAnisotropy );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glGenerateMipmap( GL_TEXTURE_2D );
	
	delete []data;

	return mainTexture;
}
