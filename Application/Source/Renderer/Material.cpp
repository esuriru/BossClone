#include "Material.h"

Material::Material(const glm::vec4& c)
	: materialShader(nullptr)
	, data(new MaterialData())
{
	data->_diffuse = c;
}

Material::Material(const glm::vec4& c, Shader* shader)
	: materialShader(shader)
	, data(new MaterialData())
{
	data->_diffuse = c;
}

Material::Material(const glm::vec4& a, const glm::vec4& d, const glm::vec4& s, const float shininess, Shader* shader)
	: materialShader(shader)
	, data(new MaterialData())
{
	data->_ambient = a;
	data->_diffuse = d;
	data->_specular = s;
	data->_shininess = shininess;
}

Material::Material()
	: data(new MaterialData())
{
	data->_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	data->_diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	data->_specular = { 0.8f, 0.8f, 0.8f, 1.0f };
}

Material::~Material()
{
	if (data)
	{
		delete data;
		data = nullptr;
	}

	//for (auto& i : textureMaps)
	//{
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	//}

	//for (auto& i : rgbTextures)
	//{
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	//}
}
