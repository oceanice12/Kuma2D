#include <RenderSystem.h>

Transform SystemManager::Render::camera;
std::vector<Entity> SystemManager::Render::entities;
std::unordered_map<Entity, Index> SystemManager::Render::entityToIndex;

std::vector<Signature> SystemManager::Render::systemSignatures =
{
	static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE),
	static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::TEXT)
};


namespace SystemManager
{
	namespace Render
	{
		SDL_Renderer* renderer;
		SDL_Window* window;
		std::unordered_map<std::string, Sprite> sprites;
		std::unordered_map<std::string, TTF_Font*> fonts;

		Sprite LoadSprite(const char* path);
		void LoadSprites(const char* path);

		TTF_Font* LoadFont(const char* path);
		void LoadFonts(const char* path);
	}
}


void SystemManager::Render::Init()
{
	int flags = FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : NULL;
	window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE.x, WINDOW_SIZE.y, flags);
	if (window == nullptr)
	{
		std::cout << "ERROR! FAILED TO CREATE WINDOW. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		std::cout << "ERROR! FAILED TO CREATE RENDERER. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(renderer, WINDOW_SIZE.x, WINDOW_SIZE.y);

	LoadSprites("res\\sprites");
	LoadFonts("res\\fonts");
}


void SystemManager::Render::Update(ComponentArray<Transform>& transforms, ComponentArray<Sprite>& sprites, ComponentArray<Text>& texts)
{
	if (SDL_RenderClear(renderer) < 0)
		std::cout << "WARNING! Failed to clear renderer. " << SDL_GetError() << std::endl;


	std::map<int, std::vector<Entity>> layerToEntities;
	for (Entity entity : entities)
	{
		int layer = 0;
		Sprite* sp = sprites[entity];
		Text* txt = texts[entity];
		
		if (sp != nullptr)
			layer = sp->layer;
		else if (txt != nullptr)
			layer = txt->layer;

		layerToEntities[layer].push_back(entity);
	}

	for (auto it = layerToEntities.begin(); it != layerToEntities.end(); it++)
	for (Entity entity : it->second)
	{
		Transform* tf = transforms[entity];
		Vector2<float> worldPos = {tf->pos.x - tf->scale.x / 2, tf->pos.y + tf->scale.y / 2};
		Vector2<float> screenPos = camera.scale * Vector2<float>{worldPos.x - camera.pos.x, camera.pos.y - worldPos.y} + WINDOW_SIZE / 2;
		SDL_Rect rect{	static_cast<int>(round(screenPos.x)),
						static_cast<int>(round(screenPos.y)),
						static_cast<int>(round(tf->scale.x * camera.scale.x)), 
						static_cast<int>(round(tf->scale.y * camera.scale.y))				};

		if (screenPos.x > WINDOW_SIZE.x || 
			screenPos.y > WINDOW_SIZE.y || 
			screenPos.x + tf->scale.x * camera.scale.x < 0 || 
			screenPos.y + tf->scale.y * camera.scale.y < 0)
			continue;

		Sprite* sp = sprites[entity];
		Text* txt = texts[entity];

		if (sp != nullptr)
			if (SDL_RenderCopy(renderer, sp->texture, NULL, &rect) < 0)
				std::cout << "WARNING! Failed to copy sprite texture to renderer. " << SDL_GetError() << std::endl;

		if (txt != nullptr)
		{
			SDL_Surface* surface = TTF_RenderText_Solid(txt->font, txt->text.c_str(), txt->color);
			if (surface == nullptr)
				std::cout << "WARNING! Failed to render text. " << SDL_GetError() << std::endl;

			SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
			if (text == nullptr)
				std::cout << "WARNING! Failed to create text texture. " << SDL_GetError() << std::endl;

			if (SDL_RenderCopy(renderer, text, NULL, &rect) < 0)
				std::cout << "WARNING! Failed to copy text texture to renderer. " << SDL_GetError() << std::endl;

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(text);
		}
	}

	SDL_RenderPresent(renderer);
}


void SystemManager::Render::LoadSprites(const char* path)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
		sprites[file.path().string()] = (LoadSprite(file.path().string().c_str()));
}

Sprite SystemManager::Render::LoadSprite(const char* path)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	if (texture == nullptr)
		std::cout << "WARNING! Failed to load file located at " << path << std::endl;

	Vector2<int> scale;
	if (SDL_QueryTexture(texture, NULL, NULL, &scale.x, &scale.y) < 0)
		std::cout << "WARNING! Failed to query file located at " << path << std::endl;


	Sprite sprite;
	sprite.scale = Vector2<float>(scale);
	sprite.texture = texture;
	return sprite;
}


void SystemManager::Render::LoadFonts(const char* path)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
		fonts[file.path().string()] = (LoadFont(file.path().string().c_str()));
}
TTF_Font* SystemManager::Render::LoadFont(const char* path)
{
	TTF_Font* font = TTF_OpenFont(path, 100);
	if (font == nullptr)
		std::cout << "WARNING! Failed to load file located at " << path << std::endl;

	return font;
}

void SystemManager::Render::FullScreen()
{
	FULLSCREEN = !FULLSCREEN;
	int flags = FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : NULL;
	SDL_SetWindowFullscreen(window, flags);
	

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(renderer, WINDOW_SIZE.x, WINDOW_SIZE.y);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
void SystemManager::Render::SetResolution(Vector2<int> resolution)
{
	WINDOW_SIZE = resolution;
	SDL_SetWindowSize(window, resolution.x, resolution.y);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(renderer, resolution.x, resolution.y);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}


TTF_Font* SystemManager::Render::GetFont(std::string path) { return fonts[path]; }
const Sprite& SystemManager::Render::GetSprite(std::string path) { return sprites[path]; }
