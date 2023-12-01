#include <Game.h>
#include <Kingdom.h>

constexpr Vector2<float> ZOOM_FACTOR{1,1};
constexpr float CAM_SPEED{100};

Entity king;
Entity background, castle, hud;
Entity camera;

void Game::Start()
{
	camera = CreateEntity(CF::TRANSFORM);
	tf(camera).pos = {0,0};
	tf(camera).scale = {1,1};

	ResourceMap rm = CreateResourceMap();

	for (int x = 0; x < MAP_SIZE.x; x++)
		for (int y = 0; y < MAP_SIZE.y; y++)
		{
			Entity e = CreateEntity(CF::TRANSFORM | CF::SPRITE);
			tf(e).scale = Vector2<float>(TILE_SIZE);
			tf(e).pos = {	float(x * TILE_SIZE.x) + (TILE_SIZE.x / 2) - (WINDOW_SIZE.x / 2),
							float(y * TILE_SIZE.y) + (TILE_SIZE.y / 2) - (WINDOW_SIZE.y / 2)};
			switch (rm[x][y])
			{
			case Resource::None:
				sp(e) = GetSprite("res\\sprites\\grass_64.png");
				SetType("grass", e);
				break;

			case Resource::Bush:
				sp(e) = GetSprite("res\\sprites\\bush_64.png");
				SetType("bush", e);
				break;

			case Resource::Tree:
				sp(e) = GetSprite("res\\sprites\\tree_64.png");
				SetType("tree", e);
				break;
			}
		}
}


void Game::Update()
{
	Camera(camera);

	// Input
	{
		// Workers
		/*
		if (Input::Mouse::GetButton(SDL_BUTTON_LEFT))
			for (Entity e : *Entities("worker"))
			{
				BoundingBox box = tf(e);
				const Vector2<int> mousePos = Input::Mouse::GetPos();
				if ((mousePos.x < box.right && mousePos.x > box.left) && (mousePos.y > box.bottom && mousePos.y < box.top))
				{
					tf(e).pos = Vector2<float>(mousePos);
				}

			}
			*/



		if (Input::Keyboard::GetKey(SDL_SCANCODE_W))
			tf(camera).pos.y += CAM_SPEED * Time::dt / tf(camera).scale.y;
		if (Input::Keyboard::GetKey(SDL_SCANCODE_S))
			tf(camera).pos.y -= CAM_SPEED * Time::dt / tf(camera).scale.y;
		if (Input::Keyboard::GetKey(SDL_SCANCODE_A))
			tf(camera).pos.x -= CAM_SPEED * Time::dt / tf(camera).scale.x;
		if (Input::Keyboard::GetKey(SDL_SCANCODE_D))
			tf(camera).pos.x += CAM_SPEED * Time::dt / tf(camera).scale.x;

		if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_P) && tf(camera).scale.x < 4)
			tf(camera).scale *= 2;
		if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_L) && tf(camera).scale.x > 1)
			tf(camera).scale *= 0.5;
		if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_R))
			tf(camera).scale = {1,1};
	}
}