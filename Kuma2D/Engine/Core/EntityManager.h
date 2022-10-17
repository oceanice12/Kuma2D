#pragma once
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <Types.h>
#include <EntityArray.h>


class EntityManager
{
public:
	EntityManager();

	Entity CreateEntity(Signature signature = 0);
	void DeleteEntity(Entity entity);

	const Signature GetSignature(Entity entity) { return signatures[entityToIndex[entity]]; }
	void SetSignature(Entity entity, Signature signature) { signatures[entityToIndex[entity]] = signature; }

	void SetType(Type type, Entity entity);
	Type GetType(Entity entity);
	const std::vector<Entity>& GetArray(Type type);

	std::vector<Entity> entities;
private:
	std::queue<Entity> idQueue;
	std::vector<Signature> signatures;
	std::unordered_map<Entity, Index> entityToIndex;
	std::unordered_map<Entity, Type> entityToType;
	std::unordered_map<Type, EntityArray> typeToArray;

	const std::vector<Entity> nullArray;
};



