#include <Game.h>
#include <Macros.h>
#include <numeric>

constexpr float PLAYER_MOVE_FORCE{ 1000 };
constexpr float ENEMY_MOVE_FORCE{ 100 };
constexpr float SWORD_SIZE{ 4 };
constexpr int MAX_ENEMIES{ 100 };

Timer timerSwordActive = { 0.3 };
Timer timerSwordAttack = { 0.5 };
Timer timerFPS = { 0.5 };

Entity camera;
Entity player;
Entity sword;

int score = 0;
Entity scoreHUD;
Entity fpsHUD;

std::vector<int> fps;

void Kuma2D::Start()
{
	// Background
	{
		Entity e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE);
		tf(e).scale = Vector2<float>(WINDOW_SIZE);
		sp(e) = GetSprite("res\\sprites\\background.png");
	}

	// Walls
	{
		Entity e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::BOX_COLLIDER);
		tf(e).scale = { 100, (float)WINDOW_SIZE.y };
		tf(e).pos = { (float)WINDOW_SIZE.x / 2, 0 };
		sp(e) = GetSprite("res\\sprites\\box.png");
		bc(e).scale = tf(e).scale;

		e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::BOX_COLLIDER);
		tf(e).scale = { 100, (float)WINDOW_SIZE.y };
		tf(e).pos = { -(float)WINDOW_SIZE.x / 2, 0};
		sp(e) = GetSprite("res\\sprites\\box.png");
		bc(e).scale = tf(e).scale;

		e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::BOX_COLLIDER);
		tf(e).scale = { (float)WINDOW_SIZE.x, 100 };
		tf(e).pos = { 0, -(float)WINDOW_SIZE.y / 2};
		sp(e) = GetSprite("res\\sprites\\box.png");
		bc(e).scale = tf(e).scale;

		e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::BOX_COLLIDER);
		tf(e).scale = { (float)WINDOW_SIZE.x, 100 };
		tf(e).pos = { 0, (float)WINDOW_SIZE.y / 2 };
		sp(e) = GetSprite("res\\sprites\\box.png");
		bc(e).scale = tf(e).scale;
	}

	// Camera
	{
		camera = CreateEntity(ComponentFlag::TRANSFORM);
	}
	
	// Player
	{
		player = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::RIGIDBODY | ComponentFlag::BOX_COLLIDER);
		tf(player).scale = { 50,50 };
		sp(player) = GetSprite("res\\sprites\\box.png");
		rb(player).drag = { 5, 0.001 };
		bc(player).scale = tf(player).scale;
	}
	
	// Sword
	{
		sword = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::CIRCLE_TRIGGER);
		tf(sword) = { tf(player).pos, {0,0} };
		sp(sword) = GetSprite("res\\sprites\\ball.png");
		ct(sword).radius = 0;
	}

	// Enemies
	{
		for (int i = 0; i < MAX_ENEMIES; i++)
			Game::SpawnEnemy();
	}
	
	// HUD
	{
		scoreHUD = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::TEXT);
		txt(scoreHUD).font = GetFont("res\\fonts\\Roboto-Bold.ttf");
		SetType("HUD", scoreHUD);

		fpsHUD = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::TEXT);
		txt(fpsHUD).font = GetFont("res\\fonts\\Roboto-Bold.ttf");
		txt(fpsHUD).color = { 0, 255, 0 };
		SetType("HUD", fpsHUD);
		timerFPS.Set(0);
	}
}


void Kuma2D::Update()
{
	if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_F11))
		FullScreen();

	// Player
	{
		// Input
		{
			Vector2<float> direction = { 0,0 };

			direction = Input::Controller::GetAxis();

			if (Input::Keyboard::GetKey(SDL_SCANCODE_W))
				direction.y += 1;
			if (Input::Keyboard::GetKey(SDL_SCANCODE_S))
				direction.y -= 1;
			if (Input::Keyboard::GetKey(SDL_SCANCODE_A))
				direction.x -= 1;
			if (Input::Keyboard::GetKey(SDL_SCANCODE_D))
				direction.x += 1;

			direction = Normalize(direction);
			Vector2<float> force = direction * PLAYER_MOVE_FORCE;

			rb(player).AddForce(force);

			if (Input::Controller::GetButtonDown(Input::Controller::Button::A) || Input::Keyboard::GetKeyDown(SDL_SCANCODE_LSHIFT))
				rb(player).AddImpulse(force);
		}

		// Collision
		{
			std::vector<Entity> collisions = GetCollisions(player);
			for (Entity col : collisions)
				if (GetType(col) == "Enemy")
					Game::Reset();
		}
	}

	// Sword
	{
		tf(sword).pos = tf(player).pos;

		// Input
		{
			if ((Input::Keyboard::GetKeyDown(SDL_SCANCODE_SPACE) || Input::Controller::GetButtonDown(Input::Controller::Button::X)) 
				&& timerSwordActive.Done() && timerSwordAttack.Done())
			{
				timerSwordActive.Reset();
				timerSwordAttack.Reset();
			}
		}

		// Collision
		{
			std::vector<Entity> collisions = GetCollisions(sword);
			Entity closest = NULL;
			for (Entity col : collisions)
			{
				if (GetType(col) == "Enemy")
				{
					if (closest == NULL || Distance(tf(player).pos, tf(col).pos) < Distance(tf(player).pos, tf(closest).pos))
						closest = col;
				}
			}

			if (closest != NULL)
			{
				DeleteEntity(closest);
				timerSwordActive.Set(0);
				timerSwordAttack.Set(0);
				score++;
			}
		}

		// Timer
		{
			if (timerSwordActive.Done())
			{
				ct(sword).radius = 0;
				tf(sword).scale = { 0,0 };
				timerSwordAttack.Tick();
			}
			else
			{
				tf(sword).scale = tf(player).scale * SWORD_SIZE;
				ct(sword).radius = tf(sword).scale.x / 2;
				timerSwordActive.Tick();
			}
		}

		//std::cout << "Active: " << timerSwordActive.time << ", Attack: " << timerSwordAttack.time << std::endl;
	}

	// Enemies
	{
		std::vector<Entity> enemies = *Entities("Enemy");

		if (enemies.size() < MAX_ENEMIES)
			for (int i = enemies.size(); i < MAX_ENEMIES; i++)
				Game::SpawnEnemy();

		for (Entity e : enemies)
		{
			Vector2<float> direction = Normalize(tf(player).pos - tf(e).pos);
			rb(e).AddForce(ENEMY_MOVE_FORCE * direction);
		}
	}

	// Camera
	{
		tf(camera).pos = tf(player).pos;
		Camera(camera);
	}

	// HUD
	{
		std::vector<Entity> hud = *Entities("HUD");
		for (Entity e : hud)
		{
			tf(e).scale = { 64.f * txt(e).text.size(), 64 };
			tf(e).pos = ScreenToWorldPos({ 0 , WINDOW_SIZE.y / 2 - 50 });
		}

		txt(scoreHUD).text = std::to_string(score);

		tf(fpsHUD).pos.x = ScreenToWorldPos({ (int)(-WINDOW_SIZE.x / 2.f + tf(fpsHUD).scale.x / 2 + 10), 0 }).x;
		timerFPS.Tick();
		fps.push_back((int)(1 / Time::dt));
		if (timerFPS.Done())
		{
			int avgFps = std::accumulate(fps.begin(), fps.end(), 0) / fps.size();
			txt(fpsHUD).text = "FPS: " + std::to_string(avgFps);
			fps.clear();
			timerFPS.Reset();
		}
	}
}



void Game::Reset()
{
	score = 0;
	timerSwordActive.Set(0);
	timerSwordAttack.Set(0);

	std::vector<Entity> entities = *Entities();
	for (Entity e : entities)
		DeleteEntity(e);

	Game::Start();
}

void Game::SpawnEnemy()
{
	Entity e = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::RIGIDBODY | ComponentFlag::BOX_TRIGGER);
	tf(e).scale = { 50,50 };
	tf(e).pos = tf(player).pos + Vector2<int>{	RangeRNG(WINDOW_SIZE.x / 2 + tf(e).scale.x, WINDOW_SIZE.x), 
												RangeRNG(WINDOW_SIZE.y / 2 + tf(e).scale.y, WINDOW_SIZE.y)};
	if (RangeRNG(0, 1) == 0)
		tf(e).pos.x *= -1;
	if (RangeRNG(0, 1) == 0)
		tf(e).pos.y *= -1;

	sp(e) = GetSprite("res\\sprites\\box.png");
	rb(e).drag = { 5, 0.001 };
	bt(e).scale = tf(e).scale * 0.8;
	SetType("Enemy", e);
}