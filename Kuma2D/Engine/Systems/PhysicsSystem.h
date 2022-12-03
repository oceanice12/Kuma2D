#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

#include <Concurrency.h>
#include <ComponentArray.h>
#include <ComponentsPlus.h>
#include <Transform.h>
#include <Rigidbody.h>
#include <Collider.h>
#include <Settings.h>
#include <Clock.h>

namespace SystemManager
{
	namespace Physics
	{
		void Update(ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies,
			ComponentArray<CircleCollider>& cColliders, ComponentArray<BoxCollider>& bColliders,
			ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers);

		extern std::unordered_multimap<Entity, Entity> collisions;

		extern std::vector<Entity> colEntities;
		extern std::unordered_map<Entity, Index> colEntityToIndex;
		extern std::vector<Signature> systemColSignatures;


		extern std::vector<Entity> rbEntities;
		extern std::unordered_map<Entity, Index> rbEntityToIndex;
		extern Signature systemRbSignature;
	}
}