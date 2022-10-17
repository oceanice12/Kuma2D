#pragma once
#include <random>

inline int RangeRNG(int start, int end)
{
	return rand() % (end - start + 1) + start;
}