#pragma once
#include <SDL.h>
#include <iostream>
#include <Vector2.h>
#include <Transform.h>
#include <Settings.h>
#include <unordered_map>

namespace Input
{
	bool Update();
	
	namespace Mouse
	{
		enum class Button
		{
			LEFT = SDL_BUTTON_LEFT,
			RIGHT = SDL_BUTTON_RIGHT
		};
		const bool GetButton(uint8_t button);
		const bool GetButtonUp(uint8_t button);
		const bool GetButtonDown(uint8_t button);
		const Vector2<int> GetPos();
	}

	namespace Keyboard
	{
		const bool GetKey(uint8_t code);
		const bool GetKeyDown(uint8_t code);
	}

	namespace Controller
	{
		enum class Button
		{
			NONE = -1,
			A = 0,
			B = 1,
			X = 2,
			Y = 3,
			LB = 4,
			RB = 5,
			BACK = 6,
			START = 7,
			LS = 8,
			RS = 9
		};
		const bool GetButton(Button button);
		const bool GetButtonDown(Button button);
		const Vector2<float> GetAxis();
	}
}

