#pragma once
#include <Vector2.h>
#include <thread>

inline Vector2<int> WINDOW_SIZE = {512, 512};
inline int WINDOW_AREA = WINDOW_SIZE.x * WINDOW_SIZE.y;

inline const char* WINDOW_NAME = "Game";
inline const int THREAD_COUNT = std::thread::hardware_concurrency() / 2;

inline Vector2<int> WORLD_SIZE = {4000, 4000};