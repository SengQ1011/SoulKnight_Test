//
// Created by tjx20 on 3/26/2025.
//

#ifndef ENUMTYPES_HPP
#define ENUMTYPES_HPP

enum class ComponentType
{
	AI,
	ANIMATION,
	ATTACK,
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

enum class ZIndexType
{
	FLOOR	= 0,	//  0 -  20
	OBJECT	= 1,	// 20 -  80
	UI		= 2,	// 80 - 100
};

#endif //ENUMTYPES_HPP
