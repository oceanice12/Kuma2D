#include <EntityManager.h>


EntityManager::EntityManager()
{
	for (int i = 1; i < MAX_ENTITIES - 1; i++)
		idQueue.push(i);
}

Entity EntityManager::CreateEntity(Signature signature)
{
	if (entities.size() > MAX_ENTITIES)
	{
		std::cout << "WARNING! Attempted to create entity while at maximum entity count." << std::endl;
		return NULL;
	}

	Entity entity = idQueue.front();
	idQueue.pop();

	entities.push_back(entity);
	signatures.push_back(signature);

	entityToIndex[entity] = entities.size() - 1;

	return entity;
}


void EntityManager::DeleteEntity(Entity entity)
{
	if (entityToIndex.find(entity) == entityToIndex.end())
	{
		std::cout << "WARNING! Attempted to delete nonexistent entity " << +entity << std::endl;
		return;
	}

	Entity lastEntity = *(entities.end() - 1);
	Signature lastSignature = *(signatures.end() - 1);
	
	entities[entityToIndex[entity]] = lastEntity;
	signatures[entityToIndex[entity]] = lastSignature;

	entityToIndex[lastEntity] = entityToIndex[entity];
	entityToIndex.erase(entity);

	entities.pop_back();
	signatures.pop_back();

	entities.shrink_to_fit();
	signatures.shrink_to_fit();

	idQueue.push(entity);


	if (entityToType.find(entity) != entityToType.end())
	{
		Type type = entityToType[entity];
		EntityArray& typeArray = typeToArray[type];
		typeArray.RemoveEntity(entity);
		entityToType.erase(entity);
		if (typeArray.GetEntities().size() == 0)
			typeToArray.erase(type);
	}
}


void EntityManager::SetType(Type type, Entity entity)
{
	if (entityToType.find(entity) != entityToType.end())
		typeToArray[entityToType[entity]].RemoveEntity(entity);

	entityToType[entity] = type;
	typeToArray[type].AddEntity(entity);
}

const std::vector<Entity>& EntityManager::GetArray(Type type)
{
	if (typeToArray.find(type) != typeToArray.end())
		return typeToArray[type].GetEntities();

	return nullArray;
}

Type EntityManager::GetType(Entity entity)
{
	if (entityToType.find(entity) != entityToType.end())
		return entityToType[entity];

	return "";
}
