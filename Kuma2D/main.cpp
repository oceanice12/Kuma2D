#include <iostream>
#include <ctime>
#include <Kuma2D.h>
#include <Kuma2d_impl.h>


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

	Kuma2D::Init();

	Kuma2D::Start(); // User scripted start

	while (Input::Update())
	{
		Kuma2D::Update();	// User scripted update
		Kuma2D::UpdateSystems();
	}


	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}