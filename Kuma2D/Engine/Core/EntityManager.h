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
}