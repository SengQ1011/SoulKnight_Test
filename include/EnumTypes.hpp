//
// Created by tjx20 on 3/26/2025.
//

#ifndef ENUMTYPES_HPP
#define ENUMTYPES_HPP

enum class ComponentType
{
	AI,
	ANIMATION,
	COLLISION,
	FOLLOWER,
	HEALTH,
	INPUT,
	MOVEMENT,
	STATE
};

enum class State
{
	STANDING,
	MOVING,
	ATTACK,
	DEAD
};

#endif //ENUMTYPES_HPP
