#include "Clock.h"

namespace Time
{
	Uint64 now;
	Uint64 last;
}

void Time::Tick()
{
	last = now;
	now = SDL_GetTicks64();
	dt = (now - last) / 1000.;
}

void Time::Wait(double seconds)
{
	Uint64 waitNow = SDL_GetTicks64();
	Uint64 waitLast = 0;
	double waitTime = 0;

	while (waitTime < seconds)
	{
		waitLast = waitNow;
		waitNow = SDL_GetTicks64();
		waitTime += (waitNow - waitLast) / 1000.;
	}

	//now += static_cast<Uint64>(seconds * 1000);
}




void Timer::Tick()
{
	if (time > 0)
		time -= Time::dt;
}

void Timer::Reset() { time = cooldown; }

void Timer::Set(float time) { this->time = time; }

bool Timer::Done() { return (time <= 0); }
