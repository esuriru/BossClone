#pragma once
#include <cstdint>
#include <bitset>
#include <queue>
#include <array>

using Entity = uint32_t;
constexpr Entity MaxEntities = 100;

using ComponentType = uint8_t;
constexpr ComponentType MaxComponents = 32;

using Signature = std::bitset<MaxComponents>;

class EntityManager
{
public:
    EntityManager();

    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    void SetSignature(Entity entity, Signature signature);
    Signature GetSignature(Entity entity);
private:
    std::queue<Entity> availableEntities_{};
    std::array<Signature, MaxEntities> signatures_{};

    uint32_t livingEntityCount_;
};