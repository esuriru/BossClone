#include <stdio.h>
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <Core/Core.h>
#include "Shader.h"
#include "Utils/Util.h"

#include <glm/gtc/type_ptr.hpp>

std::vector<std::string> Shader::s_validShaderTypes_ = {
    "vertex",
    "pixel",
    "fragment"
};

static auto ShaderTypeFromString(const std::string& type) -> GLenum
{
    if (type == "vertex") return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

    CC_ASSERT(false, "Unknown shader type.");
    return 0;
}

Shader::~Shader()
{
	glDeleteProgram(rendererID_);
}

auto Shader::ReadFile(const std::string &file_path) -> std::string
{
#ifdef CC_PLATFORM_WINDOWS
    std::string result;
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else
        // TODO: Make this not an assert.
        CC_ASSERT(false, "Could not open file: '", file_path, "'");

    return result;
#endif
}

auto Shader::Preprocess(const std::string &source) -> std::unordered_map<GLenum, std::string>
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* token = "#type";
    const size_t tokenLength = strlen(token);
    size_t pos = source.find(token, 0);

    while (pos != std::string::npos)
    {
        const size_t endOfLine = source.find_first_of("\r\n", pos);
        CC_ASSERT(endOfLine != std::string::npos, "Syntax error");
        const size_t begin = pos + tokenLength + 1; // Get the number of characters after #type declaration
        const std::string type = source.substr(begin, endOfLine - begin);

        CC_ASSERT(Utility::Contains(s_validShaderTypes_, type), "Invalid shader type specification");

        const size_t nextLinePos = source.find_first_not_of("\r\n", endOfLine);
        pos = source.find(token, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] =
            source.substr(nextLinePos,
                pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

    }

    return shaderSources;
}

auto Shader::Compile(std::unordered_map<GLenum, std::string> shaderSources) -> void
{
    GLuint program = glCreateProgram();
    CC_ASSERT(shaderSources.size() <= 2, "Nare only supports a maximum of 2 shaders for now.")
    std::array<GLenum, 2> shaderIDs;
    int shaderIDindex = 0;


    for (auto& kv_pair : shaderSources)
    {
        GLenum type = kv_pair.first;
        const std::string& source = kv_pair.second;

        // Create the shader
        GLuint shader = glCreateShader(type);

        const GLchar* source_data = source.data();

        // Set shader source
        glShaderSource(shader, 1, &source_data, nullptr);

        // Compile
        glCompileShader(shader);

        GLint isCompiled = 0;

        // Retrieve compilation status
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            // If not compiled, create a buffer to store the log.
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> errorInfoLog(maxLength);
            // Send the data from the log into the buffer
            glGetShaderInfoLog(shader, maxLength, &maxLength, errorInfoLog.data());

            glDeleteShader(shader);

            glDeleteProgram(program);

            CC_ERROR(errorInfoLog.data());

            // TODO: Add which specific shader failed compiling
            CC_ASSERT(false, "Shader compilation failure!")
            return;
        }
        // Attach the compiled shader
        glAttachShader(program, shader);
        shaderIDs[shaderIDindex++] = shader;
    }

    rendererID_ = program;

    // Link the program
    glLinkProgram(program);

    // Get its linked status
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        // If not linked, create a buffer to store the log.
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorInfoLog(maxLength);
        // Send the data from the log into the buffer
        glGetProgramInfoLog(program, maxLength, &maxLength, errorInfoLog.data());

        // Avoid memory leaks
        glDeleteProgram(program);

        for (auto& id : shaderIDs)
            glDeleteShader(id);

        CC_ERROR(errorInfoLog.data());
        CC_ASSERT(false, "OpenGLShader linking failure!")
        return;
    }

    // Detach shader after linking
    for (auto& id : shaderIDs)
        glDetachShader(program, id);
}

Shader::Shader(std::string name, const std::string& v, const std::string& f)
    : rendererID_(0)
    , name_(name)
{
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = v; 
    sources[GL_FRAGMENT_SHADER] = f; 
    Compile(sources);
}

Shader::Shader(std::string file_path)
{
    const auto& shaderSource = ReadFile(file_path);
    auto shaderSources = Preprocess(shaderSource);

    Compile(shaderSources);

    // Extract name from filepath

    // e.g Texture.glsl
    auto lastSlash = file_path.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

    auto lastDot = file_path.rfind('.');
    auto count = lastDot == std::string::npos ? file_path.size() - lastSlash : lastDot - lastSlash;

    name_ = file_path.substr(lastSlash, count);
}

auto Shader::Bind() -> void
{
	glUseProgram(rendererID_);
}

/**
 * \brief Probably will never use.
 */
auto Shader::Unbind() -> void
{
	glUseProgram(0);
}

auto Shader::SetMat4(const std::string& name, const glm::mat4& mat) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniformMatrix4fv(location.second, 1, GL_FALSE, glm::value_ptr(mat));
}

auto Shader::SetFloat(const std::string& name, const float& val) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform1f(location.second, val);
}

auto Shader::SetFloat2(const std::string& name, const glm::vec2& vec) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform2fv(location.second, 1, glm::value_ptr(vec));
}

auto Shader::SetFloat3(const std::string& name, const glm::vec3& vec) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform3fv(location.second, 1, glm::value_ptr(vec));
}

auto Shader::SetFloat4(const std::string& name, const glm::vec4& vec) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform4fv(location.second, 1, glm::value_ptr(vec));
}

auto Shader::SetInt(const std::string& name, const int& val) -> void
{
	const auto& location = TryGetLocation(name);
	if (location.first)
		glUniform1i(location.second, val);
}

auto Shader::SetIntArray(const std::string& name, int* values, uint32_t count) -> void
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
