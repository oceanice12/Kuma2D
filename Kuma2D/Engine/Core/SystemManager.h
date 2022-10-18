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
	extern void Init();
	extern void Update();
	extern void UpdateEntityArrays(Entity entity, Signature signature);
}