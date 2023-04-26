#pragma once 

#include <iostream>
#include <string>
#include "glad/glad.h"
#include <map>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader(std::string name, const std::string& v, const std::string& f);
	Shader(std::string file_path); 
	~Shader();

	auto Bind() -> void;
	auto Unbind() -> void;

	auto SetMat4(const std::string& name, const glm::mat4& mat) -> void;

	auto SetFloat(const std::string& name, const float& val) -> void;
	auto SetFloat2(const std::string& name, const glm::vec2& vec) -> void;
	auto SetFloat3(const std::string& name, const glm::vec3& vec) -> void;
	auto SetFloat4(const std::string& name, const glm::vec4& vec) -> void;

	auto SetInt(const std::string& name, const int& val) -> void;
	auto SetIntArray(const std::string& name, int* values, uint32_t count) -> void;

    inline auto GetName() const -> const std::string&
    {
        return name_;
    }

    inline auto GetID() const -> uint32_t
    {
        return rendererID_; 
    }

private:
	uint32_t rendererID_;
    std::string name_;

    auto ReadFile(const std::string& file_path) -> std::string;
    auto Preprocess(const std::string& source) -> std::unordered_map<GLenum, std::string>;
    auto Compile(std::unordered_map<GLenum, std::string> shaderSources) -> void;
    static std::vector<std::string> s_validShaderTypes_;

	friend class ShaderLibrary;

	std::unordered_map<std::string, GLint> shaderLocationCache_;
	std::pair<bool, GLint> TryGetLocation(const std::string& name);
};

