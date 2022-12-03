#pragma once
#include <Clock.h>
#include <Settings.h>
#include <vector>
#include <ComponentManager.h>
#include <RenderSystem.h>
#include <PhysicsSystem.h>
#include <AudioSystem.h>

namespace SystemManager
{
	void Init();
	void Update();
	void UpdateEntityArrays(Entity entity, Signature signature);
}