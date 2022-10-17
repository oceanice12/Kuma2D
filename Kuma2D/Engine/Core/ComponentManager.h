#pragma once
#include <ComponentArray.h>
#include <ComponentsPlus.h>
#include <Transform.h>
#include <Sprite.h>
#include <Rigidbody.h>
#include <Collider.h>
#include <Text.h>

class ComponentManager
{
public:
	ComponentManager();

	template<typename T> ComponentArray<T>& GetArray() { return *std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()]); }

	template<typename T> T& GetComponent(Entity entity) { return *std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->GetComponent(entity); }
	
	template<typename T> void AddComponent(Entity entity) { std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->AddComponent(entity); }

	template<typename T> void DeleteComponent(Entity entity) { std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->DeleteComponent(entity); }

	void AddComponents(Entity entity, Signature signature);
	void DeleteComponents(Entity entity, Signature signature);
private:
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;
};
