#include <ECS.h>

Entity ECS::CreateEntity()
{
	Entity entity = entityManager.CreateEntity();
	return entity;
}

Entity ECS::CreateEntity(Signature signature)
{
	Entity entity = entityManager.CreateEntity(signature);
	systemManager.UpdateEntityArrays(entity, signature);
	componentManager.AddComponents(entity, signature);

	return entity;
}

Entity ECS::CreateEntity(ComponentFlag flag)
{
	Signature signature = static_cast<Signature>(flag);
	Entity entity = entityManager.CreateEntity(signature);
	systemManager.UpdateEntityArrays(entity, signature);
	componentManager.AddComponents(entity, signature);

	return entity;
}

void ECS::DeleteEntity(Entity entity)
{
	Signature signature = entityManager.GetSignature(entity);
	entityManager.DeleteEntity(entity);
	systemManager.UpdateEntityArrays(entity, 0);
	componentManager.DeleteComponents(entity, signature);
}

void ECS::UpdateSystems()
{
	ECS::systemManager.Update(componentManager);
}


void ECS::Camera(Entity entity)
{
	if ((entityManager.GetSignature(entity) & ComponentFlag::TRANSFORM) == 0)
		return;

	systemManager.renderSystem.camera = GetComponent<Transform>(entity);
}

Vector2<float> ECS::ScreenToWorldPos(Vector2<int> screenPos)
{
	return {static_cast<Vector2<float>>(screenPos) / systemManager.renderSystem.camera.scale + systemManager.renderSystem.camera.pos};
}

void ECS::PlayAudio(std::string path, int loops)
{
	systemManager.audioSystem.PlayAudio(path, loops);
}

const std::vector<Entity> ECS::GetCollisions(Entity entity)
{
	std::unordered_multimap<Entity, Entity>& colPairs = systemManager.physicsSystem.collisions;
	std::vector<Entity> collisions;

	for (auto it = colPairs.find(entity), end = colPairs.end(); it != end && it->first == entity; it++)
		collisions.push_back(it->second);

	return collisions;
}



void ECS::SetType(Type type, Entity entity)
{
	entityManager.SetType(type, entity);
}

const std::vector<Entity>& ECS::Entities(Type type)
{
	if (type == "")
		return entityManager.entities;

	return entityManager.GetArray(type);
}

Type ECS::GetType(Entity entity)
{
	return entityManager.GetType(entity);
}

TTF_Font* ECS::GetFont(std::string path)
{
	return systemManager.renderSystem.GetFont(path);
}

const Sprite& ECS::GetSprite(std::string path)
{ 
	return systemManager.renderSystem.GetSprite(path); 
}

void ECS::FullScreen()
{
	systemManager.renderSystem.FullScreen();
}
void ECS::SetResolution(Vector2<int> resolution)
{
	systemManager.renderSystem.SetResolution(resolution);
}