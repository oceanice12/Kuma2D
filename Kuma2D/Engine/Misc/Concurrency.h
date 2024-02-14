#pragma once
#include <future>
#include <thread>
#include <mutex>
#include <Types.h>


namespace MultiThread
{
	std::vector<std::vector<Entity>> ParallelSplit(const std::vector<Entity>& entities, int splits);
}


// Splits an array of entities into equal or near equal sizes
inline std::vector<std::vector<Entity>> MultiThread::ParallelSplit(const std::vector<Entity>& entities, int splits)
{
	std::vector<std::vector<Entity>> subEntities;
	for (size_t i = 0, size = entities.size(), subSize = static_cast<size_t>(std::ceil(static_cast<double>(size / splits))); i < size; i += subSize)
	{
		if (subSize == 0)
			return std::vector<std::vector<Entity>>{entities};

		subEntities.push_back({entities.begin() + i, entities.begin() + std::min<size_t>(i + subSize, size)});
	}

	return subEntities;
}