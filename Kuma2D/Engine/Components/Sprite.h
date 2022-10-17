#pragma once
#include <SDL.h>
#include <Vector2.h>

struct Sprite
{
	Vector2<float> scale = {0,0};
	SDL_Texture* texture = nullptr;
	int layer = 0;
};