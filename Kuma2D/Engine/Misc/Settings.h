#pragma once
#include <Vector2.h>
#include <thread>

inline Vector2<int> WINDOW_SIZE = {700, 800};

// Change to function to allow update for area
// In the case that size changes
inline int WINDOW_AREA = WINDOW_SIZE.x * WINDOW_SIZE.y;

inline bool FULLSCREEN = false;
inline const char* WINDOW_NAME = "Game";
inline const int THREAD_COUNT = std::thread::hardware_concurrency() / 2;