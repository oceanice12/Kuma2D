#pragma once
#include <Transform.h>
#include <Sprite.h>
#include <Rigidbody.h>
#include <Collider.h>



struct BoundingBox
{
	float left{0}, right{0}, top{0}, bottom{0};

	template <typename T>
	BoundingBox(T tf)
	{ 
		left = tf.pos.x - tf.scale.x / 2;
		right = tf.pos.x + tf.scale.x / 2;
		top = tf.pos.y + tf.scale.y / 2;
		bottom = tf.pos.y - tf.scale.y / 2;
	}
};

inline bool Overlapping(BoundingBox b1, BoundingBox b2)
{
	return b1.left < b2.right && b1.right > b2.left && b1.bottom < b2.top && b1.top > b2.bottom;
}

inline Vector2<float> AreaToDimensions(float area, Vector2<float> scale)
{
	Vector2<float> scaleRatio = Normalize(scale);
	float z = sqrtf(area / (scaleRatio.x * scaleRatio.y));

	return scaleRatio * z;
}


