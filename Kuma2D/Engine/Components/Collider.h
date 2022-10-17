#pragma once
#include <Vector2.h>

struct BoxCollider
{
	Vector2<float> pos{0,0}, scale{1,1};
};

struct BoxTrigger
{
	Vector2<float> pos{0,0}, scale{1,1};
};



struct CircleCollider
{
	Vector2<float> pos{0,0};
	float radius{1};
};

struct CircleTrigger
{
	Vector2<float> pos{0,0};
	float radius{1};
};
