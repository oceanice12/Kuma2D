#pragma once
#include <SDL.h>


namespace Time
{
	inline double dt;
	void Tick();
	void Wait(double seconds);
}


struct Timer
{
	Timer(float cooldown) : cooldown{ cooldown }, time{ 0 } {}

	double cooldown;

	void Tick();
	void Reset();
	void Set(float time);
	bool Done();
private:
	double time;
};
