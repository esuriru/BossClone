#pragma once
#include <array>
#include "Core/Core.h"
#include "Shader.h"
#include "Singleton.h"

class ShaderLibrary 
{
public:
	ShaderLibrary() = default;
	~ShaderLibrary() = default;

	auto Add(const Ref<Shader>& shader) -> void;
	auto Add(const std::string& name, Ref<Shader> shader) -> void;

	auto Load(const std::string& name, const std::string& v, const std::string& f) -> Ref<Shader>;
	auto Load(const std::string& file_path) -> Ref<Shader>;

	auto Get(const std::string& name) -> Ref<Shader>;
private:
    std::unordered_map<std::string, Ref<Shader>> shaders_;

    auto Exists(const std::string& name) -> bool;
};

