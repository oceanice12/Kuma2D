#pragma once
#include <Types.h>
#include <vector>
#include <unordered_map>


class EntityArray
{
public:
	void AddEntity(Entity entity);
	void RemoveEntity(Entity entity);

	const std::vector<Entity>& GetEntities();
private:
	std::vector<Entity> entities;
	std::unordered_map<Entity, Index> entityToIndex;
};

