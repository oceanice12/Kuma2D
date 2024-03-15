#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <mutex>

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
		void Init();
		void Update(ComponentArray<Transform>& transforms, ComponentArray<Sprite>& sprites, ComponentArray<Text>& texts);
		const Sprite& GetSprite(std::string path);
		TTF_Font* GetFont(std::string path);
		void FullScreen(bool setting);
		void SetResolution(Vector2<int> resolution);

		extern Transform camera;
		extern std::vector<Entity> entities;
		extern std::unordered_map<Entity, Index> entityToIndex;
		extern std::vector<Signature> systemSignatures;
		extern std::mutex mutex;
	}
}