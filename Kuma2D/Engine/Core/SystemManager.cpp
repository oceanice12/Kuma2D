#include <SystemManager.h>

void SystemManager::Update(ComponentManager& cm)
{
	Time::Tick();

	physicsSystem.Update(	cm.GetArray<Transform>(), 
							cm.GetArray<Rigidbody>(), 
							cm.GetArray<CircleCollider>(), 
							cm.GetArray<BoxCollider>(), 
							cm.GetArray<CircleTrigger>(), 
							cm.GetArray<BoxTrigger>(),
							Time::dt);


	renderSystem.Update(	cm.GetArray<Transform>(), 
							cm.GetArray<Sprite>(), 
							cm.GetArray<Text>());
}


void SystemManager::UpdateEntityArrays(Entity entity, Signature signature)
{
	for (Signature s : renderSystem.systemSignatures)
	{
		UpdateEntityArray(renderSystem.entities, renderSystem.entityToIndex, s, entity, signature);
		if ((signature & s) == s)
			break;
	}

	UpdateEntityArray(physicsSystem.rbEntities, physicsSystem.rbEntityToIndex, physicsSystem.systemRbSignature, entity, signature);

	for (Signature s : physicsSystem.systemColSignatures)
	{
		UpdateEntityArray(physicsSystem.colEntities, physicsSystem.colEntityToIndex, s, entity, signature);
		if ((signature & s) == s)
			break;
	}
}



void SystemManager::UpdateEntityArray(std::vector<Entity>& entities, std::unordered_map<Entity, Index>& entityToIndex, Signature systemSignature, Entity entity, Signature signature)
{
	bool inArray = entityToIndex.find(entity) != entityToIndex.end();

	// Add entity to system array
	if ((signature & systemSignature) == systemSignature && !inArray)
	{
		entities.push_back(entity);
		entityToIndex[entity] = entities.size() - 1;
	}
	// Remove entity from system array
	else if ((signature & systemSignature) != systemSignature && inArray)
	{
		// Move last entity into deleted entity's index
		entities[entityToIndex[entity]] = *(entities.end() - 1);

		// Set moved entity's new index mapping to deleted entity's index
		entityToIndex[*(entities.end() - 1)] = entityToIndex[entity];

		// Delete last index holding duplicate of moved entity
		entityToIndex.erase(entity);
		entities.pop_back();
		entities.shrink_to_fit();
	}
}