#include <PhysicsSystem.h>


namespace SystemManager
{
	namespace Physics
	{
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
							ComponentArray<CircleTrigger>& cTriggers, ComponentArray<BoxTrigger>& bTriggers, 
							double dt)
{

	collisions.clear();

	for (Entity entity : rbEntities)
	{
		Transform* tf = transforms[entity];
		Rigidbody* rb = rigidbodies[entity];
		
		Vector2<float> direction = Normalize(rb->vel);
		float speed = Magnitude(rb->vel);

		Vector2<float> dragForce = -direction * (rb->drag.x * speed + rb->drag.y * speed * speed);

		Vector2<float> prevPos = tf->pos;

		rb->vel += (rb->acc * dt);
		tf->pos += (rb->vel * dt) + (rb->acc * 0.5f * dt * dt);
		rb->acc = dragForce / rb->mass + Vector2<float>{0, -rb->gravity};
	}

	for (Entity e1 : colEntities)
	for (Entity e2 : colEntities)
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

		Vector2<float> colPos = tf1->pos;
		Vector2<float> otherColPos = tf2->pos;

		enum class CollisionType
		{
			NONE,
			CIRCLE_CIRCLE,
			CIRCLE_BOX,
			BOX_CIRCLE,
			BOX_BOX
		};

		CollisionType type = CollisionType::NONE;
		if ((circCol1 != nullptr || circTrig1 != nullptr) && (circCol2 != nullptr || circTrig2 != nullptr))
			type = CollisionType::CIRCLE_CIRCLE;
		else if ((circCol1 != nullptr || circTrig1 != nullptr) && (boxCol2 != nullptr || boxTrig2 != nullptr))
			type = CollisionType::CIRCLE_BOX;
		else if ((boxCol1 != nullptr || boxTrig1 != nullptr) && (circCol2 != nullptr || circTrig2 != nullptr))
			type = CollisionType::BOX_CIRCLE;
		else if ((boxCol1 != nullptr || boxTrig1 != nullptr) && (boxCol2 != nullptr || boxTrig2 != nullptr))
			type = CollisionType::BOX_BOX;

		bool isCol = (circCol1 != nullptr || boxCol1 != nullptr);
		bool otherIsCol = (circCol2 != nullptr || boxCol2 != nullptr);


		
		switch (type)
		{
			case
			CollisionType::CIRCLE_CIRCLE:
			{
				float r1 = isCol ? circCol1->radius : circTrig1->radius;
				float r2 = otherIsCol ? circCol2->radius : circTrig2->radius;
				colPos = isCol ? circCol1->pos : circTrig1->pos;
				colPos += tf1->pos;
				otherColPos = otherIsCol ? circCol2->pos : circTrig2->pos;
				otherColPos += tf2->pos;
				if (Distance(colPos, otherColPos) >= r1 + r2)
					continue;
			}
			break;


			case
			CollisionType::CIRCLE_BOX:
			{
				colPos += isCol ? circCol1->pos : circTrig1->pos;
				otherColPos += otherIsCol ? boxCol2->pos : boxTrig2->pos;
				float r = isCol ? circCol1->radius : circTrig1->radius;
				BoundingBox box = otherIsCol ? Transform{otherColPos, boxCol2->scale} : Transform{otherColPos, boxTrig2->scale};

				Vector2<float> closest = {std::clamp(colPos.x, box.left, box.right), std::clamp(colPos.y, box.bottom, box.top)};
				Vector2<float> distance = colPos - closest;

				float distanceSquared = (distance.x * distance.x) + (distance.y * distance.y);
				if (distanceSquared >= (r * r))
					continue;
			}
			break;

			case
			CollisionType::BOX_CIRCLE:
			{
				colPos += isCol ? boxCol1->pos : boxTrig1->pos;
				otherColPos += otherIsCol ? circCol2->pos : circTrig2->pos;
				float r = otherIsCol ? circCol2->radius : circTrig2->radius;
				BoundingBox box = isCol ? Transform{colPos, boxCol1->scale} : Transform{colPos, boxTrig1->scale};

				Vector2<float> closest = {std::clamp(otherColPos.x, box.left, box.right), std::clamp(otherColPos.y, box.bottom, box.top)};
				Vector2<float> distance = otherColPos - closest;

				float distanceSquared = (distance.x * distance.x) + (distance.y * distance.y);
				if (distanceSquared >= (r * r))
					continue;
			}
			break;

			case
			CollisionType::BOX_BOX:
			{
				BoundingBox box = isCol ? Transform{colPos, boxCol1->scale} : Transform{colPos, boxTrig1->scale};
				BoundingBox otherBox = otherIsCol ? Transform{otherColPos, boxCol2->scale} : Transform{otherColPos, boxTrig2->scale};
				if (!Overlapping(box, otherBox))
					continue;
			}
			break;
		}


		collisions.insert({e1, e2});


		if (!isCol || !otherIsCol)
			continue;

		bool dynamic_dynamic = rb1 != nullptr && rb2 != nullptr;
		bool dynamic_static = (rb1 != nullptr && rb2 == nullptr) || (rb1 == nullptr && rb2 != nullptr);
		bool static_static = rb1 == nullptr && rb2 == nullptr;
		
		switch (type)
		{
			case
			CollisionType::CIRCLE_CIRCLE:
			{
				Vector2<float> direction = Normalize(colPos - otherColPos);
				Vector2<float> displacement = (circCol1->radius + circCol2->radius) * direction;

				if (dynamic_dynamic)
				{
					float pValue = 2 * (DotProduct(rb1->vel, direction) - DotProduct(rb2->vel, direction)) / (rb1->mass + rb2->mass);
					rb1->vel -= (pValue * rb2->mass * direction);
					rb2->vel += (pValue * rb1->mass * direction);
					tf2->pos = colPos - circCol1->pos - displacement / 2;
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

				tf1->pos = otherColPos - circCol2->pos + displacement / 2;
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
				BoundingBox box = Transform{colPos, boxCol1->scale};
				BoundingBox otherBox = Transform{otherColPos, boxCol2->scale};
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

				Vector2<float> dp = {0,0};
				if (prevPos.x < otherTf.pos.x)
					dp.x = otherBox.left - box.right;
				else if (prevPos.x > otherTf.pos.x)
					dp.x = otherBox.right - box.left;

				if (prevPos.y < otherTf.pos.y)
					dp.y = otherBox.bottom - box.top;
				else if (prevPos.y > otherTf.pos.y)
					dp.y = otherBox.top - box.bottom;


				Vector2<float> timeUntilCollision = {vel.x != 0 ? std::abs(dp.x / vel.x) : std::numeric_limits<float>::infinity(),
														vel.y != 0 ? std::abs(dp.y / vel.y) : std::numeric_limits<float>::infinity()};

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
