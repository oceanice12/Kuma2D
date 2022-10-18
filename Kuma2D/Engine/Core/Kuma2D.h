#pragma once
#include <RNG.h>
#include <Input.h>
#include <Types.h>
#include <Clock.h>
#include <EntityManager.h>
#include <ComponentManager.h>
#include <SystemManager.h>


namespace Kuma2D
{
	void Start();
	void Update();

	Entity							CreateEntity();
	Entity							CreateEntity(Signature signature);
	Entity							CreateEntity(ComponentFlag flag);
	void							DeleteEntity(Entity entity);

	const std::vector<Entity>*		Entities(Type type = "");
	Type							GetType(Entity entity);
	void							SetType(Type type, Entity entity);

	void							Camera(Entity entity);
	Vector2<float>					ScreenToWorldPos(Vector2<int> screenPos);

	const Sprite&					GetSprite(std::string path);
	TTF_Font*						GetFont(std::string path);
	const std::vector<Entity>		GetCollisions(Entity entity);

	template<typename T> void		AddComponent(Entity entity);
	template<typename T> T&			GetComponent(Entity entity);
	template<typename T> void		DeleteComponent(Entity entity);

	void							PlayAudio(std::string path, int loops = 0);

	void							FullScreen();
	void							SetResolution(Vector2<int> resolution);
}


template<typename T>
inline void Kuma2D::AddComponent(Entity entity)
{
	Signature oldSignature = EntityManager::GetSignature(entity);
	Signature newSignature = oldSignature | TypeToFlag<T>();
	if (oldSignature == newSignature)
	{
		std::cout << "WARNING! Attempted to add existing component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		return;
	}

	EntityManager::SetSignature(entity, newSignature);
	ComponentManager::AddComponent<T>(entity);
	SystemManager::UpdateEntityArrays(entity, newSignature);
}

template<typename T>
inline void Kuma2D::DeleteComponent(Entity entity)
{
	Signature oldSignature = EntityManager::GetSignature(entity);
	Signature newSignature = oldSignature & ~TypeToFlag<T>();
	if (oldSignature == newSignature)
	{
		std::cout << "WARNING! Attempted to delete nonexistent component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		return;
	}

	EntityManager::SetSignature(entity, newSignature);
	ComponentManager::DeleteComponent<T>(entity);
	SystemManager::UpdateEntityArrays(entity, newSignature);
}

template<typename T>
inline T& Kuma2D::GetComponent(Entity entity)
{
	Signature signature = EntityManager::GetSignature(entity);

	if ((signature & TypeToFlag<T>()) == 0)
	{
		std::cout << "WARNING! Attempted to get nonexistent component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		AddComponent<T>(entity);
		std::cout << "Added component " << typeid(T).name() << " to entity " << +entity << '.' << std::endl;
	}
	return ComponentManager::GetComponent<T>(entity);
}