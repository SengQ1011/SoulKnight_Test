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
	SKILL,
	STATE,
	TALENT
};

enum class CharacterType
{
	PLAYER,
	ENEMY,
	NPC
};

enum class AIType {
	ATTACK,		// 追蹤玩家
	SUMMON,		// 召喚
	WANDER		// 游蕩
};

enum class State
{
	STANDING,
	MOVING,
	SKILL,
	DEAD
};

enum class ManagerTypes
{
	BULLET,
	ROOMCOLLISION,
	INPUT,
	SCENE
};

enum ZIndexType: int
{
	FLOOR		= 0,	//  0 -  20
	OBJECTLOW	= 20,	// 20 -  40
	BULLET		= 40,	// 40 -  60
	OBJECTHIGH	= 60,	// 60 -  80
	UI			= 80,	// 80 - 100
};

#endif //ENUMTYPES_HPP
