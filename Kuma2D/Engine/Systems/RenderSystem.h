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

namespace SystemManager
{
	namespace Render
	{
		extern void Init();
		extern void Update(ComponentArray<Transform>& transforms, ComponentArray<Sprite>& sprites, ComponentArray<Text>& texts);
		extern const Sprite& GetSprite(std::string path);
		extern TTF_Font* GetFont(std::string path);
		extern void FullScreen();
		extern void SetResolution(Vector2<int> resolution);

		extern Transform camera;
		extern std::vector<Entity> entities;
		extern std::unordered_map<Entity, Index> entityToIndex;
		extern std::vector<Signature> systemSignatures;
	}
}