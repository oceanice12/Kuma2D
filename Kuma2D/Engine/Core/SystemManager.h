#pragma once
#include <ComponentManager.h>
#include <RenderSystem.h>
#include <PhysicsSystem.h>
#include <AudioSystem.h>
#include <Clock.h>
#include <Settings.h>
#include <vector>

class SystemManager
{
public:
	void Update(ComponentManager& cm);

	void UpdateEntityArrays(Entity entity, Signature signature);

	RenderSystem renderSystem;
	PhysicsSystem physicsSystem;
	AudioSystem audioSystem;
private:
	void UpdateEntityArray(std::vector<Entity>& entities, std::unordered_map<Entity, Index>& entityToIndex, 
							Signature systemSignature, Entity entity, Signature signature);
};