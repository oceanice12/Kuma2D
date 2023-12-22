#include <PhysicsSystem.h>

namespace SystemManager
{
	namespace Physics
	{
		void RigidbodyUpdate(std::vector<Entity> entities, ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies);

		void ColliderUpdate(	std::vector<Entity> entities, 
								ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies,
								ComponentArray<CircleCollider>& cColliders, ComponentArray<BoxCollider>& bColliders,
								ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers);

		

		enum class CollisionType
		{
			NONE,
			CIRCLE_CIRCLE,
			CIRCLE_BOX,
			BOX_CIRCLE,
			BOX_BOX
		};

		std::unordered_multimap<Entity, Entity> collisions;
		std::vector<Entity> colEntities;
		std::unordered_map<Entity, Index> colEntityToIndex;
		std::vector<Entity> rbEntities;
		std::unordered_map<Entity, Index> rbEntityToIndex;

		Signature systemRbSignature = static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::RIGIDBODY);
		std::vector<Signature> systemColSignatures =
		{
			static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::BOX_COLLIDER),
			static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::CIRCLE_COLLIDER),
			static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::BOX_TRIGGER),
			static_cast<Signature>(ComponentFlag::TRANSFORM | ComponentFlag::CIRCLE_TRIGGER)
		};
	}
}



void SystemManager::Physics::Update(	ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies, 
										ComponentArray<CircleCollider>& cColliders, ComponentArray<BoxCollider>& bColliders,
										ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers)
{
	std::vector<std::thread> threads;

	// Kinematics
	{
		std::vector<std::vector<Entity>> rbSplit = MultiThread::ParallelSplit(rbEntities, THREAD_COUNT);
		for (auto& sub : rbSplit)
		{
			std::thread t{ RigidbodyUpdate, sub, std::ref(transforms), std::ref(rigidbodies) };
			threads.push_back(std::move(t));
		}

		for (auto& t : threads)
			t.join();

		threads.clear();
	}



	// Collisions
	{
		collisions.clear();

		std::queue<Entity> colUpdates;
		for (auto col : colEntities)
			colUpdates.push(col);

		ColliderUpdate(colEntities, transforms, rigidbodies, cColliders, bColliders, cTriggers, bTriggers);
	}
}


void SystemManager::Physics::RigidbodyUpdate(std::vector<Entity> entities, ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies)
{
	for (Entity e : entities)
	{
		Transform* tf = transforms[e];
		Rigidbody* rb = rigidbodies[e];

		Vector2<float> direction = Normalize(rb->vel);
		float speed = Magnitude(rb->vel);

		Vector2<float> dragForce = -direction * (rb->drag.x * speed + rb->drag.y * speed * speed);

		Vector2<float> prevPos = tf->pos;

		rb->vel += (rb->acc * Time::dt);
		tf->pos += (rb->vel * Time::dt) + (rb->acc * 0.5f * Time::dt * Time::dt);
		rb->acc = dragForce / rb->mass + Vector2<float>{0, -rb->gravity};
	}
}

void SystemManager::Physics::ColliderUpdate(	std::vector<Entity> entities,
												ComponentArray<Transform>& transforms, ComponentArray<Rigidbody>& rigidbodies,
												ComponentArray<CircleCollider>& cColliders, ComponentArray<BoxCollider>& bColliders,
												ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers)
{
	for (Entity e1 : entities)
	for (Entity e2 : entities)
	{
		if (e1 == e2)
			continue;

		Transform* tf1 = transforms[e1];
		Transform* tf2 = transforms[e2];

		if (Distance(tf1->pos, tf2->pos) > Magnitude(tf1->scale + tf2->scale))
			continue;


		Rigidbody* rb1 = rigidbodies[e1];
		Rigidbody* rb2 = rigidbodies[e2];

		CircleCollider* circCol1 = cColliders[e1];
		BoxCollider* boxCol1 = bColliders[e1];
		CircleTrigger* circTrig1 = cTriggers[e1];
		BoxTrigger* boxTrig1 = bTriggers[e1];

		CircleCollider* circCol2 = cColliders[e2];
		BoxCollider* boxCol2 = bColliders[e2];
		CircleTrigger* circTrig2 = cTriggers[e2];
		BoxTrigger* boxTrig2 = bTriggers[e2];


		

		CollisionType type = CollisionType::NONE;
		if ((circCol1 != nullptr || circTrig1 != nullptr) && (circCol2 != nullptr || circTrig2 != nullptr))
			type = CollisionType::CIRCLE_CIRCLE;
		else if ((circCol1 != nullptr || circTrig1 != nullptr) && (boxCol2 != nullptr || boxTrig2 != nullptr))
			type = CollisionType::CIRCLE_BOX;
		else if ((boxCol1 != nullptr || boxTrig1 != nullptr) && (circCol2 != nullptr || circTrig2 != nullptr))
			type = CollisionType::BOX_CIRCLE;
		else if ((boxCol1 != nullptr || boxTrig1 != nullptr) && (boxCol2 != nullptr || boxTrig2 != nullptr))
			type = CollisionType::BOX_BOX;


		Vector2<float> colPos1 = tf1->pos;
		Vector2<float> colPos2 = tf2->pos;
		bool isCol1 = (circCol1 != nullptr || boxCol1 != nullptr);
		bool isCol2 = (circCol2 != nullptr || boxCol2 != nullptr);

		switch (type)
		{
			case
			CollisionType::CIRCLE_CIRCLE:
			{
				float r1 = isCol1 ? circCol1->radius : circTrig1->radius;
				float r2 = isCol2 ? circCol2->radius : circTrig2->radius;

				colPos1 += isCol1 ? circCol1->pos : circTrig1->pos;
				colPos2 += isCol2 ? circCol2->pos : circTrig2->pos;

				if (Distance(colPos1, colPos2) >= r1 + r2)
					continue;
			}
			break;


			case
			CollisionType::CIRCLE_BOX:
			{
				colPos1 += isCol1 ? circCol1->pos : circTrig1->pos;
				colPos2 += isCol2 ? boxCol2->pos : boxTrig2->pos;
				float r = isCol1 ? circCol1->radius : circTrig1->radius;
				BoundingBox box = isCol2 ? Transform{colPos2, boxCol2->scale} : Transform{colPos2, boxTrig2->scale};

				Vector2<float> closest = {std::clamp(colPos1.x, box.left, box.right), std::clamp(colPos1.y, box.bottom, box.top)};
				Vector2<float> distance = colPos1 - closest;

				float distanceSquared = (distance.x * distance.x) + (distance.y * distance.y);
				if (distanceSquared >= (r * r))
					continue;
			}
			break;

			case
			CollisionType::BOX_CIRCLE:
			{
				colPos1 += isCol1 ? boxCol1->pos : boxTrig1->pos;
				colPos2 += isCol2 ? circCol2->pos : circTrig2->pos;
				float r = isCol2 ? circCol2->radius : circTrig2->radius;
				BoundingBox box = isCol1 ? Transform{colPos1, boxCol1->scale} : Transform{colPos1, boxTrig1->scale};

				Vector2<float> closest = {std::clamp(colPos2.x, box.left, box.right), std::clamp(colPos2.y, box.bottom, box.top)};
				Vector2<float> distance = colPos2 - closest;

				float distanceSquared = (distance.x * distance.x) + (distance.y * distance.y);
				if (distanceSquared >= (r * r))
					continue;
			}
			break;

			case
			CollisionType::BOX_BOX:
			{
				BoundingBox box = isCol1 ? Transform{colPos1, boxCol1->scale} : Transform{colPos1, boxTrig1->scale};
				BoundingBox otherBox = isCol2 ? Transform{colPos2, boxCol2->scale} : Transform{colPos2, boxTrig2->scale};
				if (!Overlapping(box, otherBox))
					continue;
			}
			break;
		}

		collisions.insert(std::make_pair(e1,e2));

		if (!isCol1 || !isCol2)
			continue;

		bool dynamic_dynamic = rb1 != nullptr && rb2 != nullptr;
		bool dynamic_static = (rb1 != nullptr && rb2 == nullptr) || (rb1 == nullptr && rb2 != nullptr);
		bool static_static = rb1 == nullptr && rb2 == nullptr;

		// Collisions Resolution
		switch (type)
		{
			case
			CollisionType::CIRCLE_CIRCLE:
			{
				Vector2<float> direction = Normalize(colPos1 - colPos2);
				Vector2<float> displacement = (circCol1->radius + circCol2->radius) * direction;

				if (dynamic_dynamic)
				{
					float pValue = 2 * (DotProduct(rb1->vel, direction) - DotProduct(rb2->vel, direction)) / (rb1->mass + rb2->mass);
					rb1->vel -= (pValue * rb2->mass * direction);
					rb2->vel += (pValue * rb1->mass * direction);
					tf2->pos = colPos1 - circCol1->pos - displacement / 2;
				}
				else if (dynamic_static)
				{
					Rigidbody* rb = rb1 != nullptr ? rb1 : rb2;
					float pValue = 2 * (DotProduct(rb->vel, direction)) / (rb->mass);
					rb->vel -= (pValue * rb->mass * direction);
				}
				else if (static_static)
				{

				}

				tf1->pos = colPos2 - circCol2->pos + displacement / 2;
			}
			break;

			case
			CollisionType::CIRCLE_BOX:
			{
			}
			break;

			case
			CollisionType::BOX_CIRCLE:
			{
			}
			break;


			case
			CollisionType::BOX_BOX:
			{
				BoundingBox box = Transform{ colPos1, boxCol1->scale };
				BoundingBox otherBox = Transform{ colPos2, boxCol2->scale };
				Transform tf = *tf1;
				Transform otherTf = *tf2;

				if (static_static)
					continue;

				Vector2<float> vel = { 0,0 };
				if (rb1 != nullptr)
				{
					vel = rb1->vel;
				}
				else if (rb2 != nullptr)
				{
					vel = rb2->vel;
					std::swap(tf, otherTf);
					std::swap(box, otherBox);
				}

				Vector2<float> prevPos = tf.pos - vel * 1;

				Vector2<float> dp = { 0,0 };
				if (prevPos.x < otherTf.pos.x)
					dp.x = otherBox.left - box.right;
				else if (prevPos.x > otherTf.pos.x)
					dp.x = otherBox.right - box.left;

				if (prevPos.y < otherTf.pos.y)
					dp.y = otherBox.bottom - box.top;
				else if (prevPos.y > otherTf.pos.y)
					dp.y = otherBox.top - box.bottom;


				Vector2<float> timeUntilCollision = { vel.x != 0 ? std::abs(dp.x / vel.x) : std::numeric_limits<float>::infinity(),
														vel.y != 0 ? std::abs(dp.y / vel.y) : std::numeric_limits<float>::infinity() };

				if (timeUntilCollision.x < timeUntilCollision.y)
				{
					if (dp.x > 0)
						tf.pos.x = otherBox.right + tf.scale.x / 2;
					else if (dp.x < 0)
						tf.pos.x = otherBox.left - tf.scale.x / 2;

					//rb.acc.x = 0;
					//rb.vel.x = 0;
				}
				else if (timeUntilCollision.y < timeUntilCollision.x)
				{
					if (dp.y > 0)
						tf.pos.y = otherBox.top + tf.scale.y / 2;
					else if (dp.y < 0)
						tf.pos.y = otherBox.bottom - tf.scale.y / 2;

					//rb.acc.y = 0;
					//rb.vel.y = 0;
				}

				if (rb1 != nullptr)
					*tf1 = tf;
				else if (rb2 != nullptr)
					*tf2 = tf;
			}
			break;
		}
	}
}