#pragma once
#include <ECS.h>


class Game : public ECS
{
public:
	void Start() override;
	void Update() override;


	void Reset();

	// Balls
	void SpawnBall();


	// Space invaders
	void Player();
	void Aliens();
	void Bullets();
	void GameStateCheck();

	// Tower fall
	void SpawnObstacle();
	void SpawnPowerup();
};