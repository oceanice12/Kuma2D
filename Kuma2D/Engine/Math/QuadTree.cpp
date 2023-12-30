#include <QuadTree.h>



QuadTree::QuadTree(Transform transform) : transform{transform}, box{transform}, northWest{ nullptr }, northEast{nullptr}, southWest{nullptr}, southEast{nullptr} {}

bool QuadTree::Insert(Entity e, BoundingBox b)
{
	Vector2<float> topLeft{b.left,b.top};
	Vector2<float> topRight{b.right,b.top};
	Vector2<float> bottomLeft{b.left,b.bottom};
	Vector2<float> bottomRight{b.right,b.bottom};

	Node nTopLeft{topLeft, e};
	Node nTopRight{topRight, e};
	Node nBottomLeft{bottomLeft, e};
	Node nBottomRight{bottomRight, e};

	return (InsertNode(nTopLeft) && InsertNode(nTopRight) && InsertNode(nBottomLeft) && InsertNode(nBottomRight));
}

bool QuadTree::InsertNode(Node node)
{
	if (!box.Contains(node.pos))
		return false;

	if (nodes.size() < CAPACITY)
	{
		nodes.push_back(node);
		return true;
	}

	Subdivide();

	if (northWest->InsertNode(node)) return true;
	if (northEast->InsertNode(node)) return true;
	if (southWest->InsertNode(node)) return true;
	if (southEast->InsertNode(node)) return true;

	return false;
}

std::vector<Entity> QuadTree::Query(BoundingBox range)
{
	std::vector<Entity> found;
	if (!Overlapping(box, range))
		return found;

	for (const auto& node : nodes)
		if (range.Contains(node.pos))
			found.push_back(node.entity);

	if (northWest == nullptr)
		return found;

	std::vector<std::vector<Entity>> temp;
	temp.push_back(northWest->Query(range));
	temp.push_back(northEast->Query(range));
	temp.push_back(southWest->Query(range));
	temp.push_back(southEast->Query(range));
	for (const auto& v : temp)
		found.insert(found.end(), v.begin(), v.end());

	return found;
}

void QuadTree::Subdivide()
{
	Transform tf;
	tf.scale = transform.scale / 2;

	tf.pos = transform.pos + Vector2<float>{-1, 1} * tf.scale / 2;
	northWest = std::unique_ptr<QuadTree>(new QuadTree(tf));

	tf.pos = transform.pos + Vector2<float>{1, 1} * tf.scale / 2;
	northEast = std::unique_ptr<QuadTree>(new QuadTree(tf));

	tf.pos = transform.pos + Vector2<float>{-1, -1} * tf.scale / 2;
	southWest = std::unique_ptr<QuadTree>(new QuadTree(tf));

	tf.pos = transform.pos + Vector2<float>{1, -1} * tf.scale / 2;
	southEast = std::unique_ptr<QuadTree>(new QuadTree(tf));
}

void QuadTree::Remove(Entity e)
{
	for (int i = 0, s = nodes.size(); i < s; i++)
	{
		if (nodes[i].entity == e)
		{
			nodes[i] = nodes.back();
			nodes.pop_back();
		}
	}

	if (northWest == nullptr)
		return;

	northWest->Remove(e);
	northEast->Remove(e);
	southWest->Remove(e);
	southEast->Remove(e);

	if (northWest->nodes.empty() && northEast->nodes.empty() && southWest->nodes.empty() && southEast->nodes.empty())
	{
		northWest.reset();
		northEast.reset();
		southWest.reset();
		southEast.reset();
	}
}