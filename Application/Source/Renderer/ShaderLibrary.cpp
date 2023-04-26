#include "ShaderLibrary.h"

auto ShaderLibrary::Add(const Ref<Shader> &shader) -> void
{
    const auto& name = shader->GetName();
    Add(name, shader);
}

auto ShaderLibrary::Add(const std::string &name, Ref<Shader> shader) -> void
{
    if (!Exists(name))
    {
        CC_ERROR("Shader with name '", name, " 'already exists!" );
        return;
    }
    shaders_[name] = shader;
}

auto ShaderLibrary::Load(const std::string &name, const std::string &v, const std::string &f) -> Ref<Shader>
{
    auto shader = CreateRef<Shader>(name, v, f);
    Add(name, shader);
    return shader;
}

auto ShaderLibrary::Load(const std::string &file_path) -> Ref<Shader>
{
    auto shader = CreateRef<Shader>(file_path);
    Add(shader);
    return shader;
}

auto ShaderLibrary::Exists(const std::string &name) -> bool
{
    return shaders_.find(name) != shaders_.end();
}
