#pragma once
#include <Kuma2D.h>
#include <array>

using namespace Kuma2D;

inline constexpr Vector2<int> MAP_SIZE = {32,32};
inline constexpr Vector2<int> TILE_SIZE = {16,16};

enum Resource
{
	None = 0,
	Bush,
	Tree,
	Rock,
	Ore
};

using ResourceMap = std::array<std::array<Resource, MAP_SIZE.y>, MAP_SIZE.x>;

ResourceMap CreateResourceMap()
{
	int rng = 0;

	ResourceMap rm;
	for (auto& row : rm)
		for (auto& resource : row)
		{
			rng = rand() % 100;
			if (rng <= 50)
				resource = Resource::None;
			else if (rng > 50 && rng < 75)
				resource = Resource::Bush;
			else
				resource = Resource::Tree;
		}

	return rm;
}

void SpawnWorker()
{
	Entity e = CreateEntity(CF::TRANSFORM | CF::SPRITE | CF::BOX_COLLIDER);

	SetType("worker", e);

	sp(e) = GetSprite("res\\sprites\\worker.png");
	sp(e).layer = 5;
	tf(e).pos = {0, -50};
	tf(e).scale = sp(e).scale;

}