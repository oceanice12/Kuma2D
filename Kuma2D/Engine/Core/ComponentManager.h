#pragma once
#include <ComponentArray.h>

namespace ComponentManager
{
	void Init();

	extern std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;


	template<typename T> ComponentArray<T>& GetArray();
	template<typename T> T& GetComponent(Entity entity);
	template<typename T> void AddComponent(Entity entity);
	template<typename T> void DeleteComponent(Entity entity);

	void AddComponents(Entity entity, Signature signature);
	void DeleteComponents(Entity entity, Signature signature);
}

template<typename T>
inline ComponentArray<T>& ComponentManager::GetArray()
{
	return *std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()]);
}

template<typename T>
inline T& ComponentManager::GetComponent(Entity entity)
{
	return *std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->GetComponent(entity);
}

template<typename T>
inline void ComponentManager::AddComponent(Entity entity)
{
	std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->AddComponent(entity);
}

template<typename T>
inline void ComponentManager::DeleteComponent(Entity entity)
{
	std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T).name()])->DeleteComponent(entity);
}