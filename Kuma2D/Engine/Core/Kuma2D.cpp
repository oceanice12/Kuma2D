#include "Kuma2D.h"

void Kuma2D::Init()
{
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (IMG_Init(IMG_INIT_PNG) < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL IMG. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (TTF_Init() < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL TEXT. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	EntityManager::Init();
	ComponentManager::Init();
	SystemManager::Init();
}

void Kuma2D::Quit()
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Kuma2D::QuitGame()
{
	SDL_Event quit;
	quit.type = SDL_QUIT;
	SDL_PushEvent(&quit);
}

Entity Kuma2D::CreateEntity()
{
	return EntityManager::CreateEntity();
}

Entity Kuma2D::CreateEntity(Signature signature)
{
	Entity entity = EntityManager::CreateEntity(signature);
	SystemManager::UpdateEntityArrays(entity, signature);
	ComponentManager::AddComponents(entity, signature);

	return entity;
}

Entity Kuma2D::CreateEntity(ComponentFlag flag)
{
	Signature signature = static_cast<Signature>(flag);
	Entity entity = EntityManager::CreateEntity(signature);
	SystemManager::UpdateEntityArrays(entity, signature);
	ComponentManager::AddComponents(entity, signature);

	return entity;
}

void Kuma2D::DeleteEntity(Entity entity)
{
	Signature signature = EntityManager::GetSignature(entity);
	EntityManager::DeleteEntity(entity);
	SystemManager::UpdateEntityArrays(entity, 0);
	ComponentManager::DeleteComponents(entity, signature);
}

void Kuma2D::UpdateSystems()
{
	SystemManager::Update();
}


void Kuma2D::Camera(Entity entity)
{
	if ((EntityManager::GetSignature(entity) & ComponentFlag::TRANSFORM) == 0)
		return;

	SystemManager::Render::camera = GetComponent<Transform>(entity);
}

Vector2<float> Kuma2D::ScreenToWorldPos(Vector2<int> screenPos)
{
	return { static_cast<Vector2<float>>(screenPos) / SystemManager::Render::camera.scale + SystemManager::Render::camera.pos };
}

void Kuma2D::PlayAudio(std::string path, int loops)
{
	SystemManager::Audio::PlayAudio(path, loops);
}

const std::vector<Entity> Kuma2D::GetCollisions(Entity entity)
{
	std::unordered_multimap<Entity, Entity>& colPairs = SystemManager::Physics::collisions;
	std::vector<Entity> collisions;

	for (auto it = colPairs.find(entity), end = colPairs.end(); it != end && it->first == entity; it++)
		collisions.push_back(it->second);

	return collisions;
}



void Kuma2D::SetType(Type type, Entity entity)
{
	EntityManager::SetType(type, entity);
}

const std::vector<Entity>* Kuma2D::Entities(Type type)
{
	if (type == "")
		return EntityManager::GetEntities();

	return EntityManager::GetArray(type);
}

Type Kuma2D::GetType(Entity entity)
{
	return EntityManager::GetType(entity);
}

TTF_Font* Kuma2D::GetFont(std::string path)
{
	return SystemManager::Render::GetFont(path);
}

const Sprite& Kuma2D::GetSprite(std::string path)
{
	return SystemManager::Render::GetSprite(path);
}

void Kuma2D::FullScreen(bool setting)
{
	SystemManager::Render::FullScreen(setting);
}
void Kuma2D::SetResolution(Vector2<int> resolution)
{
	SystemManager::Render::SetResolution(resolution);
}

void Kuma2D::SetWorldSize(Vector2<int> size)
{

}