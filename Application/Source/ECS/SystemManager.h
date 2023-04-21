#pragma once

#include "System.h"
#include <memory>
#include <unordered_map>
class SystemManager
{
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        const char* typeName = typeid(T).name();

        NR_CORE_ASSERT(systems_.find(typeName) == systems_.end(), "Registering system more than once.");

        auto system = std::make_shared<T>();
        systems_.insert({ typeName, system });
        return system;
    }

    template<typename T>
    void SetSignature(Signature signature)
    {
        const char* typeName = typeid(T).name();

        NR_CORE_ASSERT(systems_.find(typeName) != systems_.end(), "System used before registered.");

        signatures_.insert({ typeName, signature });
    }

    void EntityDestroyed(Entity entity)
    {
        for (const auto& pair : systems_)
        {
            const auto& system = pair.second;

            system->Entities.erase(entity);
        }
    }

    void EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        for (const auto& pair : systems_)
        {
            const auto& type = pair.first;
            const auto& system = pair.second;
            const auto& systemSignature = signatures_[type];

            // If the signatures match, insert
            if ((entitySignature & systemSignature) == systemSignature)
            {
                system->Entities.insert(entity);
            }
            system->Entities.erase(entity);
        }
    }

private:
    std::unordered_map<const char*, Signature> signatures_;

    std::unordered_map<const char*, std::shared_ptr<System>> systems_;
};
