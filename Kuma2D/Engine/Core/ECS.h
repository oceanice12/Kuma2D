#pragma once

#include <EntityManager.h>
#include <ComponentManager.h>
#include <SystemManager.h>
#include <RNG.h>
#include <Input.h>


class ECS
{
public:
	// User defined in Game.cpp
	/******************************************************************************************/
	virtual void					Start() = 0;
	virtual void					Update() = 0;
	/******************************************************************************************/ 


	// User Interface
	/******************************************************************************************/
	Entity							CreateEntity		();
	Entity							CreateEntity		(Signature signature);
	Entity							CreateEntity		(ComponentFlag flag);
	void							DeleteEntity		(Entity entity);

	const std::vector<Entity>&		Entities			(Type type = "");
	Type							GetType				(Entity entity);
	void							SetType				(Type type, Entity entity);

	void							Camera				(Entity entity);
	Vector2<float>					ScreenToWorldPos	(Vector2<int> screenPos);

	const Sprite&					GetSprite			(std::string path);
	TTF_Font*						GetFont				(std::string path);
	const std::vector<Entity>		GetCollisions		(Entity entity);

	template<typename T> void		AddComponent		(Entity entity);
	template<typename T> T&			GetComponent		(Entity entity);
	template<typename T> void		DeleteComponent		(Entity entity);

	void							PlayAudio			(std::string path, int loops = 0);

	void							FullScreen			();
	void							SetResolution		(Vector2<int> resolution);
	/*******************************************************************************************/


	// Systems Interface
	/*******************************************************************************************/
	void							UpdateSystems		();
	/*******************************************************************************************/
private:
	EntityManager entityManager;
	ComponentManager componentManager;
	SystemManager systemManager;
};



template<typename T> 
inline void ECS::AddComponent(Entity entity)
{
	Signature oldSignature = entityManager.GetSignature(entity);
	Signature newSignature = oldSignature | TypeToFlag<T>();
	if (oldSignature == newSignature)
	{
		std::cout << "WARNING! Attempted to add existing component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		return;
	}

	entityManager.SetSignature(entity, newSignature);
	componentManager.AddComponent<T>(entity);
	systemManager.UpdateEntityArrays(entity, newSignature);
}

template<typename T> 
inline void ECS::DeleteComponent(Entity entity)
{
	Signature oldSignature = entityManager.GetSignature(entity);
	Signature newSignature = oldSignature & ~TypeToFlag<T>();
	if (oldSignature == newSignature)
	{
		std::cout << "WARNING! Attempted to delete nonexistent component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		return;
	}

	entityManager.SetSignature(entity, newSignature);
	componentManager.DeleteComponent<T>(entity);
	systemManager.UpdateEntityArrays(entity, newSignature);
}

template<typename T> 
inline T& ECS::GetComponent(Entity entity)
{
	Signature signature = entityManager.GetSignature(entity);

	if ((signature & TypeToFlag<T>()) == 0)
	{
		std::cout << "WARNING! Attempted to get nonexistent component " << typeid(T).name() << " for entity " << +entity << '.' << std::endl;
		AddComponent<T>(entity);
		std::cout << "Added component " << typeid(T).name() << " to entity " << +entity << '.' << std::endl;
	}
	return componentManager.GetComponent<T>(entity);
}