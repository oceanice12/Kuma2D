#include "AudioSystem.h"

namespace SystemManager
{
	namespace Audio
	{
		std::unordered_map<std::string, Mix_Music*> songs;
		std::unordered_map<std::string, Mix_Chunk*> sounds;

		Mix_Music* LoadSong(const char* path);
		Mix_Chunk* LoadSound(const char* path);
		void LoadSongs(const char* path);
		void LoadSounds(const char* path);
	}
}

void SystemManager::Audio::Init()
{
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		std::cout << "ERROR! FAILED TO OPEN AUDIO DEVICE. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	LoadSongs("res\\audio\\music");
	LoadSounds("res\\audio\\sounds");
}



void SystemManager::Audio::PlayAudio(std::string audio, int loops)
{
	if (songs.find(audio) != songs.end())
	{
		if (Mix_PlayMusic(songs[audio], loops) < 0)
			std::cout << "WARNING! Failed to play audio " << audio << std::endl;

		return;
	}

	if (sounds.find(audio) != sounds.end())
	{
		if (Mix_PlayChannel(-1, sounds[audio], loops) < 0)
			std::cout << "WARNING! Failed to play audio " << audio << std::endl;

		return;
	}
}


Mix_Music* SystemManager::Audio::LoadSong(const char* path)
{
	Mix_Music* music = Mix_LoadMUS(path);
	if (music == nullptr)
		std::cout << "WARNING! Failed to load file located at " << path << std::endl;

	return music;
}

Mix_Chunk* SystemManager::Audio::LoadSound(const char* path)
{
	Mix_Chunk* sound = Mix_LoadWAV(path);
	if (sound == nullptr)
		std::cout << "WARNING! Failed to load file located at " << path << std::endl;

	return sound;
}

void SystemManager::Audio::LoadSongs(const char* path)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
		songs[file.path().string()] = (LoadSong(file.path().string().c_str()));
}

void SystemManager::Audio::LoadSounds(const char* path)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
		sounds[file.path().string()] = (LoadSound(file.path().string().c_str()));
}