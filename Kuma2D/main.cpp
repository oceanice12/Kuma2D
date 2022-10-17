#include <Game.h>
#include <iostream>
#include <ctime>


int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (IMG_Init(IMG_INIT_PNG) < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL IMG. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (TTF_Init() < 0)
	{
		std::cout << "ERROR! FAILED TO INITIALIZE SDL TEXT. " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	Game game{};
	game.Start();	// User scripted start

	while (Input::Update())
	{
		game.Update();	// User scripted update
		game.UpdateSystems();
	}


	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}