#pragma once
#include <Kuma2D.h>
#include <Macros.h>
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


/*
void SpawnResource(Type type, int count)
{
	for (int i = 0; i < count; i++)
	{
		Entity e = CreateEntity(CF::TRANSFORM | CF::SPRITE | CF::BOX_TRIGGER);
		SetType(type, e);

		sp(e) = GetSprite("res\\sprites\\" + type + ".png");
		sp(e).layer = 3;

		tf(e).pos = {float(rand() % WINDOW_SIZE.x) - WINDOW_SIZE.x / 2, float(rand() % WINDOW_SIZE.y) - (WINDOW_SIZE.y / 2)};
		tf(e).scale = sp(e).scale;
		bt(e).pos = tf(e).pos;
		bt(e).scale = tf(e).scale;

		BoundingBox box = tf(e);
		bool offScreen = (box.left < -WINDOW_SIZE.x / 2 || box.right > WINDOW_SIZE.x / 2 || box.top > WINDOW_SIZE.y / 2 || box.bottom < -WINDOW_SIZE.y / 2);
		bool colliding = true;

		int tries = 10;

		while ((offScreen || colliding) && tries > 0)
		{
			tries--;
			colliding = false;
			tf(e).pos = {float(rand() % WINDOW_SIZE.x) - WINDOW_SIZE.x / 2, float(rand() % WINDOW_SIZE.y) - (WINDOW_SIZE.y / 2)};
			box = tf(e);
			offScreen = (box.left < -WINDOW_SIZE.x / 2 || box.right > WINDOW_SIZE.x / 2 || box.top > WINDOW_SIZE.y / 2 || box.bottom < -WINDOW_SIZE.y / 2);
			bt(e).pos = tf(e).pos;

			for (auto e2 : *Entities())
			{
				if (e == e2 || GetType(e2) == "Background")
					continue;
				BoundingBox box2 = tf(e2);
				if (Overlapping(box, box2))
				{
					colliding = true;
					break;
				}
			}
		}

		if (tries == 0)
			DeleteEntity(e);
	}
}
*/

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

	return std::move(rm);
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