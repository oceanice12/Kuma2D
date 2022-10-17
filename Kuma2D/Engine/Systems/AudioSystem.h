#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <unordered_map>
#include <Types.h>
#include <filesystem>

class AudioSystem
{
public:
	AudioSystem();

	void PlayAudio(std::string audio, int loops = 0);

private:
	std::unordered_map<std::string, Mix_Music*> songs;
	std::unordered_map<std::string, Mix_Chunk*> sounds;

	Mix_Music* LoadSong(const char* path);
	Mix_Chunk* LoadSound(const char* path);
	void LoadSongs(const char* path);
	void LoadSounds(const char* path);
};

