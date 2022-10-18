#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <unordered_map>
#include <Types.h>
#include <filesystem>

namespace SystemManager
{
	namespace Audio
	{
		void Init();

		void PlayAudio(std::string audio, int loops = 0);
	}
}