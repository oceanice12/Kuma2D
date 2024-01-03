#pragma once
#include <Vector2.h>
#include <ComponentsPlus.h>
#include <vector>
#include <Types.h>

constexpr uint8_t CAPACITY = 16;


// A quadtree normally detects points only but to use it for collision detection all 4 corners of a collider must be stored in the quadtree

struct Node
{
	Vector2<float> pos;
	Entity entity;
};

class QuadTree
{
public:
	QuadTree(Transform transform);
	bool Insert(Entity entity, BoundingBox box);
	void Query(std::vector<Entity>& found, BoundingBox& range);
	void Remove(Entity entity);
private:
	void Subdivide();
	bool InsertNode(Node node);

	Transform transform;
	BoundingBox box;
	std::vector<Node> nodes;
	std::unique_ptr<QuadTree> northWest;
	std::unique_ptr<QuadTree> northEast;
	std::unique_ptr<QuadTree> southWest;
	std::unique_ptr<QuadTree> southEast;
};