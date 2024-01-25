/*
|--------------------------*   KUMA2D   *--------------------------|
| Kuma2D is a 2D GAME ENGINE built from scratch. 
| SDL2 APIs are used to handle low level I/O for easy portability.
| 
|
| Kuma2D utilizes DATA-ORIENTED DESIGN. 
|
|
| WHY NOT OBJECTED-ORIENTED DESIGN:
|
| In a traditional Object-Oriented design, data is packaged
| together into discrete classes that represent an entity in
| a game. This approach is programmer friendly because all the
| data for a game object can be found in one place. 
| However, for that very reason, Object-Oriented design is not
| computer friendly.
|
| Say for example the player inputs a movement key for their
| character to move forward. The only data we are concerned with
| is the player character's position and physics. However, because
| our game entities package ALL their data ranging from position
| to sprites into one object, we must load ALL data concerning
| our player character. This means that even though we only mean
| to manipulate the player's position, we are forced to load ALL
| of the player's data whether we need it or not.
|
| This is no real issue when our game only has a few objects
| loaded at anytime, however, performance greatly suffers
| as the size of manipulated data increases. This is where
| Data-Oriented Design comes into play.
|
|
| WHY DATA-ORIENTED DESIGN:
|
|
| Each entity is a number
| used to index into arrays of similar data. This approach is much
| faster than grouping different data into their own shared
| object such as in an object-oriented design.
|-------------------------------------------------------------------
*/








#pragma once
#include <iostream>
#include <ctime>
#include <RNG.h>
#include <Input.h>
#include <Types.h>
#include <Clock.h>
#include <EntityManager.h>
#include <ComponentManager.h>
#include <SystemManager.h>
#include <Macros.h>


namespace Kuma2D
{
	void Init();
	void UpdateSystems();
	void Quit();

	void InitNetwork(bool host);

	Entity							CreateEntity					();
	Entity							CreateEntity					(Signature signature);
	Entity							CreateEntity					(ComponentFlag flag);
	void							DeleteEntity					(Entity entity);

	const std::vector<Entity>*		Entities						(Type type = "");
	Type							GetType							(Entity entity);
	void							SetType							(Type type, Entity entity);

	void							Camera							(Entity entity);
	Vector2<float>					ScreenToWorldPos				(Vector2<int> screenPos);
	void							SetWorldSize					(Vector2<int> size);

	const Sprite&					GetSprite						(std::string path);
	TTF_Font*						GetFont							(std::string path);
	const std::vector<Entity>		GetCollisions					(Entity entity);

	template<typename T> void		AddComponent					(Entity entity);
	template<typename T> T&			GetComponent					(Entity entity);
	template<typename T> void		DeleteComponent					(Entity entity);

	void							PlayAudio						(std::string path, int loops = 0);

	void							FullScreen						(bool setting);
	void							SetResolution					(Vector2<int> resolution);
	void							QuitGame						();
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