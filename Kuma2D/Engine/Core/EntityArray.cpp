#include "EntityArray.h"



void EntityArray::AddEntity(Entity entity)
{
	entities.push_back(entity);
	entityToIndex[entity] = entities.size() - 1;
}


void EntityArray::RemoveEntity(Entity entity)
{
	Entity lastEntity = *(entities.end() - 1);

	entities[entityToIndex[entity]] = lastEntity;

	entityToIndex[lastEntity] = entityToIndex[entity];
	entityToIndex.erase(entity);

	entities.pop_back();
	entities.shrink_to_fit();
}
