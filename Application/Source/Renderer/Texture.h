#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glad/glad.h>

/// <summary>
/// All textures are bound to 0.
/// </summary>
class Texture
{
public:
	Texture(const char* tga_file_path, bool tga_file = true);
	virtual ~Texture();

	void Release();
	
	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;

	Texture(Texture&& other);
	Texture& operator=(Texture&& other);

	void Bind(uint32_t slot = 0);
	void Unbind();

	static Texture* LoadTGATexture(const char* tga_file_path);
	static Texture* LoadTexture(const char* file_path_for_stb, bool managed = true);
	static Texture* LoadCubemap(const std::vector<std::string>& file_paths);

	inline std::string GetPath()
	{
		return path_;
	}

private:
	friend class Font;
	Texture() = default;
	std::string path_;
	uint32_t id_;

	uint32_t width_;
	uint32_t height_;
	
	GLenum internalFormat_, dataFormat_;
};

