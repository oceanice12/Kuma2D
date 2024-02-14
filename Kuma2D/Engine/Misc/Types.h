#pragma once
#include <cstdint>
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include <Transform.h>
#include <Sprite.h>
#include <Rigidbody.h>
#include <Collider.h>
#include <Text.h>


using Entity = std::uint32_t;
constexpr Entity MAX_ENTITIES = 1000000;

using Index = std::uint32_t;

using Signature = std::uint8_t;

using Type = std::string;

enum class ComponentFlag
{
	NONE =				(0 << 0),
	TRANSFORM =			(1 << 0),
	SPRITE =			(1 << 1),
	RIGIDBODY =			(1 << 2),
	CIRCLE_COLLIDER =	(1 << 3),
	BOX_COLLIDER =		(1 << 4),
	BOX_TRIGGER =		(1 << 5),
	CIRCLE_TRIGGER =	(1 << 6),
	TEXT =				(1 << 7)
};

const std::vector<ComponentFlag> componentFlags = {ComponentFlag::TRANSFORM,	ComponentFlag::SPRITE,
												ComponentFlag::RIGIDBODY,	ComponentFlag::CIRCLE_COLLIDER,
												ComponentFlag::BOX_COLLIDER, ComponentFlag::BOX_TRIGGER,
												ComponentFlag::CIRCLE_TRIGGER, ComponentFlag::TEXT};

const std::vector<std::string> componentTypes = {	typeid(Transform).name(), typeid(Sprite).name(),
											typeid(Rigidbody).name(), typeid(CircleCollider).name(),
											typeid(BoxCollider).name(), typeid(BoxTrigger).name(),
											typeid(CircleTrigger).name(), typeid(Text).name()};

template<typename T>
inline ComponentFlag TypeToFlag()
{
	std::string type = typeid(T).name();

	for (int i = 0; i < componentTypes.size(); i++)
	{
		if (type == componentTypes[i])
			return componentFlags[i];
	}

	std::cout << "WARNING! Could not convert type " << type << " to a component flag." << std::endl;

	return ComponentFlag::NONE;
}


inline std::string FlagToType(ComponentFlag cf)
{
	for (int i = 0; i < componentFlags.size(); i++)
	{
		if (cf == componentFlags[i])
			return componentTypes[i];
	}

	std::cout << "WARNING! Could not convert component flag " << +static_cast<Signature>(cf) << " to a type." << std::endl;

	return "NONE";
}

inline ComponentFlag operator|(ComponentFlag a, ComponentFlag b) { return static_cast<ComponentFlag>(static_cast<int>(a) | static_cast<int>(b)); }
inline Signature operator|(Signature a, ComponentFlag b) { return static_cast<Signature>(static_cast<int>(a) | static_cast<int>(b)); }
inline bool operator&(Signature s, ComponentFlag f) { return (static_cast<int>(s) & static_cast<int>(f)); }