#pragma once
#include "Nare/Core/Core.h"

namespace Nare
{
	using Entity = uint32_t;
	constexpr Entity MaxEntities = 5000;

	using ComponentType = std::uint8_t;
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
}