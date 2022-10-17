#pragma once
#include <Vector2.h>


struct Rigidbody
{
	Vector2<float> vel{0,0}, acc{0,0}, drag{0,0};
	float mass{1}, gravity{0};

	void AddForce(Vector2<float> force)
	{
		this->acc += force / this->mass;
	}

	void AddImpulse(Vector2<float> impulse)
	{
		this->vel += impulse / this->mass;
	}
};
