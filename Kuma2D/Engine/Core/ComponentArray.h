#pragma once
#include <vector>
#include <unordered_map>
#include <Types.h>

// An interface used by the Component Manager to fit different component arrays into one manageable map
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
};

template<typename T>
struct ComponentArrayData
{
	const std::vector<T>* components;
	const std::unordered_map<Entity, Index>* entityToIndex;
	const std::unordered_map<Index, Entity>* indexToEntity;
};


template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void AddComponent(Entity entity);
	void operator +=(Entity entity);

	void DeleteComponent(Entity entity);

	T* GetComponent(Entity entity);
	T* operator [](Entity entity);

	const ComponentArrayData<T> GetData();
private:
	std::vector<T> components;
	std::unordered_map<Entity, Index> entityToIndex;
	std::unordered_map<Index, Entity> indexToEntity;
};



template<typename T>
inline const ComponentArrayData<T> ComponentArray<T>::GetData()
{
	ComponentArrayData<T> data;
	data.components = &components;
	data.entityToIndex = &entityToIndex;

	return data;
}


template<typename T>
inline void ComponentArray<T>::AddComponent(Entity entity)
{
	T component{};
	components.push_back(component);
	entityToIndex[entity] = static_cast<Index>(components.size() - 1);
	indexToEntity[static_cast<Index>(components.size() - 1)] = entity;
}


template<typename T>
inline void ComponentArray<T>::operator +=(Entity entity)
{
	AddComponent(entity);
}

template<typename T>
inline void ComponentArray<T>::DeleteComponent(Entity entity)
{
	T movedComponent = *(components.end() - 1);
	Index lastIndex = static_cast<Index>(components.size() - 1);
	Entity movedEntity = indexToEntity[lastIndex];
	Index index = entityToIndex[entity];


	components[index] = movedComponent;

	entityToIndex[movedEntity] = index;
	indexToEntity[index] = movedEntity;

	indexToEntity.erase(lastIndex);
	entityToIndex.erase(entity);
	components.pop_back();
	components.shrink_to_fit();
}

template<typename T>
inline T* ComponentArray<T>::GetComponent(Entity entity)
{
	if (entityToIndex.find(entity) == entityToIndex.end())
		return nullptr;

	return &components[entityToIndex[entity]];
}

template<typename T>
inline T* ComponentArray<T>::operator [](Entity entity)
{
	return GetComponent(entity);
}