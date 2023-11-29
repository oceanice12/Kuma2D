




if (Input::Keyboard::GetKey(SDL_SCANCODE_UP))
	tf(camera).pos.y += CAM_SPEED * Time::dt / tf(camera).scale.y;
if (Input::Keyboard::GetKey(SDL_SCANCODE_DOWN))
	tf(camera).pos.y -= CAM_SPEED * Time::dt / tf(camera).scale.y;
if (Input::Keyboard::GetKey(SDL_SCANCODE_LEFT))
	tf(camera).pos.x -= CAM_SPEED * Time::dt / tf(camera).scale.x;
if (Input::Keyboard::GetKey(SDL_SCANCODE_RIGHT))
	tf(camera).pos.x += CAM_SPEED * Time::dt / tf(camera).scale.x;

if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_P))
	tf(camera).scale += ZOOM_FACTOR * Time::dt * tf(camera).scale;
if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_L))
	tf(camera).scale -= ZOOM_FACTOR * Time::dt * tf(camera).scale;
if (Input::Keyboard::GetKeyDown(SDL_SCANCODE_R))
	tf(camera).scale = {1,1};