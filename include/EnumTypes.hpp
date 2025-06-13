//
// Created by tjx20 on 3/26/2025.
//

#ifndef ENUMTYPES_HPP
#define ENUMTYPES_HPP

enum class ComponentType
{
	AI,				 // 0
	ANIMATION,		 // 1
	ATTACK,			 // 2
	CHEST,			 // 3
	COLLISION,		 // 4
	DOOR,			 // 5
	EFFECT_ATTACK,	 // 6
	FLICKER,		 // 7
	FOLLOWER,		 // 8
	HEALTH,			 // 9
	INPUT,			 // 10
	INTERACTABLE,	 // 11
	MOVEMENT,		 // 12
	PROJECTILE,		 // 13
	SKILL,			 // 14
	SPIKE,			 // 15
	STATE,			 // 16
	TALENT,			 // 17
	WALLET,			 // 18
	DESTRUCTIBLE_EFFECT,
	DROP,
	NPC
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
	SKILL2,
	SKILL3,
	SKILL4,
	SKILL5,
	ATTACK,
	DEAD
};

enum class StatusEffect
{
	NONE,
	BURNS,		// 灼燒
	POISON,		// 中毒
	ELECTRIC,	// 感電
	DIZZINESS,	// 眩暈
	FROZEN,		// 冰凍
	FATIGUE		// 疲勞
};

enum class EffectAttackType
{
	NONE,
	SLASH,
	LUNGE,
	SHOCKWAVE,
	LARGE_SHOCKWAVE,
	ENERGY_WAVE,
	LARGE_BOOM,
	MEDIUM_BOOM,
	SMALL_BOOM,
	ICE_SPIKE,
	POISON_AREA
};

enum class AttackType
{
	EFFECT_ATTACK,
	PROJECTILE,
	COLLISION,
	NONE
};

enum class AttackStrategies
{
	NONE,
	COLLISION_ATTACK,
	MELEE,
	GUN,
	BOSS
};

enum class WeaponType
{
	NONE,
	// Melee
	SWORD,				// 刀劍
	HAMMER,				// 錘子
	AXEnCLUB,			// 斧頭or棍棒
	SPEAR,				// 長槍/長矛
	// Gun
	PISTOL,				// 手槍
	RIFLE,				// 步槍
	SNIPER,				// 狙擊槍
	SHOTGUN,			// 霰彈槍
	ROCKET_LAUNCHER,	// 火箭炮
	BOWnCROSSBOW,		// 弓or弩
	STAFF,				// 法杖
};


enum class MonsterType
{
	WANDER, // 游蕩
	ATTACK, // 追蹤玩家
	SUMMON, // 召喚
	BOSS
};

enum class enemyState
{
	IDLE,
	WANDERING,
	CHASING,
	READY_ATTACK,
	SKILL1,
	SKILL2,
	SKILL3,
	SKILL4,
	SKILL5
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
	FLOOR		= 0,	//  0 -  20
	OBJECTLOW	= 20,	// 20 -  40
	ATTACK		= 40,	// 40 -  60
	OBJECTHIGH	= 60,	// 60 -  80
	UI			= 80,	// 80 -  99
	CUSTOM		=100,   // dont care
};

enum class ChestType
{
	REWARD,
	WEAPON
};

enum class MineType
{
	ENERGY,
	GOLD
};

enum class InteractableType
{
	NONE,
	PORTAL,
	SHOP_TABLE,
	NPC_DIALOGUE,
	REWARD_CHEST,
	WEAPON_CHEST,
	COIN,
	ENERGY_BALL,
	WEAPON,
	HP_POISON,
	ENERGY_POISON,
	NPC_MERCHANT,
	NPC_RICH_GUY,
	NPC_HELLO_WORLD,
	GASHAPON_MACHINE,
	EGG
};

enum class PotionType
{
	HEALTH,
	ENERGY
};

enum class PotionSize
{
	SMALL,
	BIG
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
