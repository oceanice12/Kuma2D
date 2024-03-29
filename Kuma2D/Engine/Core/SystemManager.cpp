#include <SystemManager.h>

namespace SystemManager
{
	enum class Action
	{
		REMOVED = -1,
		ADDED = 1,
		NONE = 0
	};

	Action UpdateEntityArray(std::vector<Entity>& entities, std::unordered_map<Entity, Index>& entityToIndex,
		Signature systemSignature, Entity entity, Signature signature);
}

void SystemManager::Init()
{
	Render::Init();
	Audio::Init();
}

void SystemManager::Update()
{
	Time::Tick();

	Physics::Update(		ComponentManager::GetArray<Transform>(), 
							ComponentManager::GetArray<Rigidbody>(), 
							ComponentManager::GetArray<CircleCollider>(), 
							ComponentManager::GetArray<BoxCollider>(), 
							ComponentManager::GetArray<CircleTrigger>(), 
							ComponentManager::GetArray<BoxTrigger>());


	Render::Update(			ComponentManager::GetArray<Transform>(), 
							ComponentManager::GetArray<Sprite>(), 
							ComponentManager::GetArray<Text>());
}


void SystemManager::UpdateEntityArrays(Entity entity, Signature signature)
{
	for (Signature s : Render::systemSignatures)
	{
		UpdateEntityArray(Render::entities, Render::entityToIndex, s, entity, signature);
		if ((signature & s) == s)
			break;
	}

	UpdateEntityArray(Physics::rbEntities, Physics::rbEntityToIndex, Physics::systemRbSignature, entity, signature);

	for (Signature s : Physics::systemColSignatures)
	{
		Action action = UpdateEntityArray(Physics::colEntities, Physics::colEntityToIndex, s, entity, signature);

		switch (action)
		{
		case Action::REMOVED:
			Physics::quadtreeRemovals.push_back(entity);
			break;

		case Action::ADDED:
			Physics::quadtreeInsertions.push_back(entity);
			break;
		}

		if ((signature & s) == s)
			break;
	}
}



SystemManager::Action SystemManager::UpdateEntityArray(std::vector<Entity>& entities, std::unordered_map<Entity, Index>& entityToIndex, Signature systemSignature, Entity entity, Signature signature)
{
	bool inArray = entityToIndex.find(entity) != entityToIndex.end();

	// Add entity to system array
	if ((signature & systemSignature) == systemSignature && !inArray)
	{
		entities.push_back(entity);
		entityToIndex[entity] = static_cast<Index>(entities.size() - 1);

		return SystemManager::Action::ADDED;
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

		return SystemManager::Action::REMOVED;
	}

	return SystemManager::Action::NONE;
}