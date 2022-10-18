#pragma once
#include <ComponentManager.h>

std::unordered_map<const char*, std::shared_ptr<IComponentArray>> ComponentManager::componentArrays;

void ComponentManager::Init()
{
	componentArrays[typeid(Transform).name()] = std::make_shared<ComponentArray<Transform>>();
	componentArrays[typeid(Sprite).name()] = std::make_shared<ComponentArray<Sprite>>();
	componentArrays[typeid(Rigidbody).name()] = std::make_shared<ComponentArray<Rigidbody>>();
	componentArrays[typeid(CircleCollider).name()] = std::make_shared<ComponentArray<CircleCollider>>();
	componentArrays[typeid(BoxCollider).name()] = std::make_shared<ComponentArray<BoxCollider>>();
	componentArrays[typeid(BoxTrigger).name()] = std::make_shared<ComponentArray<BoxTrigger>>();
	componentArrays[typeid(CircleTrigger).name()] = std::make_shared<ComponentArray<CircleTrigger>>();
	componentArrays[typeid(Text).name()] = std::make_shared<ComponentArray<Text>>();
}



void ComponentManager::AddComponents(Entity entity, Signature signature)
{
	if (signature & ComponentFlag::TRANSFORM)
		AddComponent<Transform>(entity);

	if (signature & ComponentFlag::SPRITE)
		AddComponent<Sprite>(entity);

	if (signature & ComponentFlag::RIGIDBODY)
		AddComponent<Rigidbody>(entity);

	if (signature & ComponentFlag::CIRCLE_COLLIDER)
		AddComponent<CircleCollider>(entity);

	if (signature & ComponentFlag::BOX_COLLIDER)
		AddComponent<BoxCollider>(entity);

	if (signature & ComponentFlag::BOX_TRIGGER)
		AddComponent<BoxTrigger>(entity);

	if (signature & ComponentFlag::CIRCLE_TRIGGER)
		AddComponent<CircleTrigger>(entity);

	if (signature & ComponentFlag::TEXT)
		AddComponent<Text>(entity);
}

void ComponentManager::DeleteComponents(Entity entity, Signature signature)
{
	if (signature & ComponentFlag::TRANSFORM)
		DeleteComponent<Transform>(entity);

	if (signature & ComponentFlag::SPRITE)
		DeleteComponent<Sprite>(entity);

	if (signature & ComponentFlag::RIGIDBODY)
		DeleteComponent<Rigidbody>(entity);

	if (signature & ComponentFlag::CIRCLE_COLLIDER)
		DeleteComponent<CircleCollider>(entity);

	if (signature & ComponentFlag::BOX_COLLIDER)
		DeleteComponent<BoxCollider>(entity);

	if (signature & ComponentFlag::BOX_TRIGGER)
		DeleteComponent<BoxTrigger>(entity);

	if (signature & ComponentFlag::CIRCLE_TRIGGER)
		DeleteComponent<CircleTrigger>(entity);

	if (signature & ComponentFlag::TEXT)
		DeleteComponent<Text>(entity);
}