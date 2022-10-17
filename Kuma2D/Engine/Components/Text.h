#pragma once
#include <SDL_ttf.h>
#include <string>
#include <SDL.h>

struct Text
{
	TTF_Font* font = nullptr;
	SDL_Color color = {255, 255, 255};
	std::string text = " ";
	int layer = 0;
};
