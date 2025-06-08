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
	CHEST,
	COLLISION,
	DOOR,
	DESTRUCTIBLE_EFFECT,
	DROP,
	FLICKER,
	FOLLOWER,
	HEALTH,
	INPUT,
	INTERACTABLE,
	MOVEMENT,
	PROJECTILE,
	SKILL,
	STATE,
	TALENT,
	WALLET
};

enum class CharacterType
{
	PLAYER,
	ENEMY,
	NPC,
	NEUTRAL // 中性，不分敵我
};

enum class State
{
	STANDING,
	MOVING,
	SKILL,
	ATTACK,
	DEAD
};

enum class StatusEffect
{
	NONE,
	BURNS, // 灼燒
	POISON, // 中毒
	ELECTRIC, // 感電
	DIZZINESS, // 眩暈
	FROZEN, // 冰凍
	FATIGUE // 疲勞
};

enum class EffectAttackType
{
	NONE,
	SLASH,
	LUNGE,
	SHOCKWAVE,
	ENERGY_WAVE,
	LARGE_BOOM,
	MEDIUM_BOOM,
	SMALL_BOOM
};

enum class AttackType
{
	EFFECT_ATTACK,
	PROJECTILE,
	COLLISION,
	NONE
};

enum class WeaponType
{
	NONE,
	// Melee
	SWORD, // 刀劍
	HAMMER, // 錘子
	AXEnCLUB, // 斧頭or棍棒
	SPEAR, // 長槍/長矛
	// Gun
	PISTOL, // 手槍
	RIFLE, // 步槍
	SNIPER, // 狙擊槍
	SHOTGUN, // 霰彈槍
	ROCKET_LAUNCHER, // 火箭炮
	BOWnCROSSBOW, // 弓or弩
	STAFF, // 法杖
};


enum class MonsterType
{
	ATTACK, // 追蹤玩家
	SUMMON, // 召喚
	WANDER // 游蕩
};

enum class enemyState
{
	IDLE,
	WANDERING,
	CHASING,
	READY_ATTACK
};

enum class ManagerTypes
{
	ATTACK,
	ROOMCOLLISION,
	ROOMINTERACTIONMANAGER,
	INPUT,
	SCENE,
	TRACKING
};

enum ZIndexType : int
{
	FLOOR = 0, //  0 -  20
	OBJECTLOW = 20, // 20 -  40
	ATTACK = 40, // 40 -  60
	OBJECTHIGH = 60, // 60 -  80
	UI = 80, // 80 -  99
	CUSTOM = 100, // dont care
};

enum class ChestType
{
	REWARD,
	WEAPON
};

enum class InteractableType
{
	NONE,
	PORTAL,
	SHOP,
	NPC_DIALOGUE,
	REWARD_CHEST,
	WEAPON_CHEST,
	COIN,
	ENERGY_BALL,
	WEAPON,
	HP_POISON,
	ENERGY_POISON
};

enum class EntityCategory
{
	PLAYER, // 玩家角色
	ENEMY, // 敵人
	NPC, // NPC
	OTHER // 其他實體
};

enum class CombatState
{
	INACTIVE, // 戰鬥未開始
	WAVE_ACTIVE, // 當前波次進行中
	WAVE_CLEARING, // 當前波次清理完畢，準備下一波
	COMPLETED // 所有波次完成
};

#endif // ENUMTYPES_HPP
