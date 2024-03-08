#pragma once
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <Types.h>
#include <EntityArray.h>


namespace EntityManager
{
	void Init();

	Entity CreateEntity(Signature signature = 0);
	void DeleteEntity(Entity entity);

	const Signature GetSignature(Entity entity);
	void SetSignature(Entity entity, Signature signature);

	void SetType(Type type, Entity entity);
	Type GetType(Entity entity);

	const std::vector<Entity>* GetArray(Type type);
	const std::vector<Entity>* GetEntities();

	struct EntityGameState
	{
		const std::vector<Entity>* entities;
		const std::queue<Entity>* idQueue;
		const std::vector<Signature>* signatures;
		const std::unordered_map<Entity, Index>* entityToIndex;
		const std::unordered_map<Entity, Type>* entityToType;
		const std::unordered_map<Type, EntityArray>* typeToArray;
	};

	const EntityGameState GetGameState();
}