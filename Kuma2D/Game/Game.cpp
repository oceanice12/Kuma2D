#include <Game.h>
#include <Macros.h>
#include <Kingdom.h>

Entity king;
Entity background, castle, hud;

void Kuma2D::Start()
{
	hud = CreateEntity(CF::TRANSFORM | CF::SPRITE);
	SetType("HUD", hud);
	sp(hud) = GetSprite("res\\sprites\\hud.png");
	sp(hud).layer = 100;
	tf(hud).pos = {0, 350};
	tf(hud).scale = sp(hud).scale;

	background = CreateEntity(CF::TRANSFORM | CF::SPRITE);
	SetType("Background", background);
	sp(background) = GetSprite("res\\sprites\\grass.png");
	sp(background).layer = -1;
	tf(background).pos = {0,-50};
	tf(background).scale = sp(background).scale;

	castle = CreateEntity(CF::TRANSFORM | CF::SPRITE | CF::BOX_COLLIDER);
	SetType("Castle", castle);
	sp(castle) = GetSprite("res\\sprites\\castle.png");
	sp(castle).layer = 0;
	tf(castle).pos = {0,-50};
	tf(castle).scale = sp(castle).scale;
	bc(castle).pos = tf(castle).pos;
	bc(castle).scale = tf(castle).scale;

	SpawnResource("worker", 1);

	SpawnResource("bush", 15);
	SpawnResource("tree", 5);
	SpawnResource("rock", 5);
	SpawnResource("goldrock", 1);
}


void Kuma2D::Update()
{
	// Input
	{
		// Workers
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

		if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_SPACE))
			SpawnResource("bush", 1);
	}
}