#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Shader.h"
#include "Core/Core.h"

#include <glm/gtc/type_ptr.hpp>

#if 1
	#define MUTE_SHADER
#endif

Shader* Shader::DefaultShader;
Shader* Shader::MappedShader;
Shader* Shader::MultitexturedShader;
Shader* Shader::BillboardShader;

Shader::~Shader()
{
	glDeleteProgram(rendererID_);
}

GLuint Shader::LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		CC_FATAL("Cannot open this shader: ", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	CC_INFO("Compiling shader : ", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	CC_INFO("Compiling shader : ", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		CC_ERROR(std::string(FragmentShaderErrorMessage.begin(), FragmentShaderErrorMessage.end()));
	}



	// Link the program
	CC_INFO("Linking shaders.");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		CC_ERROR(std::string(ProgramErrorMessage.begin(), ProgramErrorMessage.end()));
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}



Shader::Shader(const char* v, const char* f)
	: rendererID_(LoadShaders(v, f))
	, flags_(0)
{
}

void Shader::Bind()
{
	glUseProgram(rendererID_);
}

/**
 * \brief Probably will never use.
 */
void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniformMatrix4fv(location.second, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetFloat(const std::string& name, const float& val)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform1f(location.second, val);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& vec)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform2fv(location.second, 1, glm::value_ptr(vec));
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& vec)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform3fv(location.second, 1, glm::value_ptr(vec));
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& vec)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform4fv(location.second, 1, glm::value_ptr(vec));
}

void Shader::SetInt(const std::string& name, const int& val)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform1i(location.second, val);
}

void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform1iv(location.second, count, values);
}

std::pair<bool, GLint> Shader::TryGetLocation(const std::string& name)
{
	const auto& i = shaderLocationCache_.find(name);
	if (i == shaderLocationCache_.end())
	{
		const auto& location = glGetUniformLocation(rendererID_, name.data());
		if (location == -1)
		{
#ifndef MUTE_SHADER
			std::cout << "Uniform location not found with name '" << name << "'\n";
#endif
			return { false, 0 };
		}
		shaderLocationCache_[name] = location;
		return { true, location };
	}
	return { true, i->second };
}
