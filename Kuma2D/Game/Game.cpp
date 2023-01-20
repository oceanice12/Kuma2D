#include <Game.h>
#include <Macros.h>


Entity player;

void Kuma2D::Start()
{
	player = CreateEntity(ComponentFlag::TRANSFORM | ComponentFlag::SPRITE | ComponentFlag::RIGIDBODY);
	tf(player).scale = { 10,10 };
	tf(player).pos = { 0,-300 };
	sp(player) = GetSprite("res\\sprites\\box.png");
}


void Kuma2D::Update()
{

}