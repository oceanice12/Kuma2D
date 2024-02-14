#include "Input.h"

namespace Input
{
	namespace Mouse
	{
		void Update();
		void Event(const SDL_Event& event);

		Vector2<int> pos;
		std::unordered_map<uint8_t, bool> buttonsDown;
		std::unordered_map<uint8_t, bool> buttonsUp;
		uint8_t buttons;
		int scroll;
	}

	namespace Keyboard
	{
		void Update();
		void Event(const SDL_Event& event);

		// All keys pressed down in a given tick. Resets every tick.
		std::unordered_map<uint8_t, bool> keyboardDown;

		// Keyboard state. 1 = down, 0 = up.
		const uint8_t* keyboard;
	}

	namespace Controller
	{
		void Update();
		void Event(const SDL_Event& event);

		SDL_Joystick* joystick;
		Vector2<float> axis;
		Button button;
		Button buttonDown;
	}
}



const bool Input::Mouse::GetButton(uint8_t button) { return (buttons & button); }
const bool Input::Mouse::GetButtonUp(uint8_t button) { return buttonsUp[button]; };
const bool Input::Mouse::GetButtonDown(uint8_t button) { return buttonsDown[button]; }
const Vector2<int> Input::Mouse::GetPos() { return pos; };
const int Input::Mouse::GetScroll() { return scroll; }

void Input::Mouse::Event(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		buttonsDown[event.button.button] = true;
		break;

	case SDL_MOUSEBUTTONUP:
		buttonsUp[event.button.button] = true;
		break;

	case SDL_MOUSEWHEEL:
		scroll = event.wheel.y;
		break;
	}
}
void Input::Mouse::Update()
{
	buttonsDown.clear();
	buttonsUp.clear();

	buttons = SDL_GetMouseState(&Mouse::pos.x, &Mouse::pos.y);
	pos -= WINDOW_SIZE / 2;
	pos.y *= -1;

	scroll = 0;
}


const bool Input::Keyboard::GetKey(uint8_t code) { return keyboard[code]; }
const bool Input::Keyboard::GetKeyDown(uint8_t code) { return keyboardDown[code]; }
void Input::Keyboard::Event(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		keyboardDown[event.key.keysym.scancode] = !event.key.repeat;
		break;
	}
}
void Input::Keyboard::Update()
{
	keyboardDown.clear();
	keyboard = SDL_GetKeyboardState(NULL);
}


const bool Input::Controller::GetButton(Button button) { return button == Controller::button; }
const bool Input::Controller::GetButtonDown(Button button) { return button == buttonDown; }
const Vector2<float> Input::Controller::GetAxis() { return axis; }
void Input::Controller::Event(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_JOYDEVICEADDED:
	{
		joystick = SDL_JoystickOpen(event.jdevice.which);
		if (joystick == nullptr)
			std::cout << "WARNING! Unable to open game controller. " << SDL_GetError() << std::endl;
		else
			std::cout << "Controller detected." << std::endl;
	}
	break;

	case SDL_JOYDEVICEREMOVED:
	{
		joystick = SDL_JoystickFromInstanceID(event.jdevice.which);
		SDL_JoystickClose(joystick);
		joystick = nullptr;
	}
	break;

	case SDL_JOYBUTTONDOWN:
	{
		joystick = SDL_JoystickFromInstanceID(event.jbutton.which);
		buttonDown = static_cast<Controller::Button>(event.jbutton.button);
		button = static_cast<Controller::Button>(event.jbutton.button);
	}
	break;
	}
}
void Input::Controller::Update()
{
	buttonDown = Controller::Button::NONE;
	if (Controller::joystick != nullptr)
	{
		Controller::axis = { static_cast<float>(SDL_JoystickGetAxis(Controller::joystick, 0)),
							static_cast<float>(SDL_JoystickGetAxis(Controller::joystick, 1)) };
		int deadZone = 10000;

		if (!(Controller::axis.x < -deadZone || Controller::axis.x > deadZone))
			Controller::axis.x = 0;
		if (!(Controller::axis.y < -deadZone || Controller::axis.y > deadZone))
			Controller::axis.y = 0;

		Controller::axis.y *= -1;
	}
}


bool Input::Update()
{
	Mouse::Update();
	Keyboard::Update();
	Controller::Update();

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
			Controller::Event(event);
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			Keyboard::Event(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
			Mouse::Event(event);
		}
	}
	return true;
}
