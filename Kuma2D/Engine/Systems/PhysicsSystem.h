#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <ComponentArray.h>
#include <ComponentsPlus.h>
#include <Transform.h>
#include <Rigidbody.h>
#include <Collider.h>
#include <Settings.h>


class PhysicsSystem
{
public:
	void Update(ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies,
				ComponentArray<CircleCollider>& cColliders, ComponentArray<BoxCollider>& bColliders,
				ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers,
				double dt);

	std::unordered_multimap<Entity, Entity> collisions;

	std::vector<Entity> colEntities;
	std::unordered_map<Entity, Index> colEntityToIndex;
	std::vector<Signature> systemColSignatures = 
	{
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::BOX_COLLIDER), 
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::CIRCLE_COLLIDER),
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::BOX_TRIGGER), 
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::CIRCLE_TRIGGER)
	};


	std::vector<Entity> rbEntities;
	std::unordered_map<Entity, Index> rbEntityToIndex;
	Signature systemRbSignature = static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::RIGIDBODY);
};