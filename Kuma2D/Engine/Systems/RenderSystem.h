#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>

#include <ComponentArray.h>
#include <ComponentsPlus.h>
#include <Transform.h>
#include <Sprite.h>
#include <Text.h>
#include <Vector2.h>
#include <Settings.h>


class RenderSystem
{
public:
	RenderSystem();

	void Update(ComponentArray<Transform>& transforms, ComponentArray<Sprite>& sprites, ComponentArray<Text>& texts);

	Transform camera;

	const Sprite& GetSprite(std::string path) { return sprites[path]; }
	TTF_Font* GetFont(std::string path) { return fonts[path]; }

	std::vector<Entity> entities;
	std::unordered_map<Entity, Index> entityToIndex;

	std::vector<Signature> systemSignatures = 
	{
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE),
		static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::TEXT)
	};

	void FullScreen();
	void SetResolution(Vector2<int> resolution);
private:
	SDL_Renderer* renderer;
	SDL_Window* window;
	std::unordered_map<std::string, Sprite> sprites;
	std::unordered_map<std::string, TTF_Font*> fonts;

	Sprite LoadSprite(const char* path);
	void LoadSprites(const char* path);

	TTF_Font* LoadFont(const char* path);
	void LoadFonts(const char* path);
};

