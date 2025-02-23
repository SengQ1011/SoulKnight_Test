#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
#include <vector>
#include "Weapon.hpp"
#include "Talent.hpp"
#include "Skill.hpp"
#include "StatusEffect.hpp"

class Character {
public:
	// 屬性
	int maxHealth;        // 生命上限
	int currentHealth;    // 當前生命值
	int maxArmor;         // 護甲上限
	int currentArmor;     // 當前護甲值
	int maxEnergy;        // 能量上限
	int currentEnergy;    // 當前能量值

private:
	double critRate;      // 暴擊率（0-1）
	int handBladeDamage;  // 手刀傷害
	double moveSpeed;     // 每秒移動的格數
	int aimRange;         // 自動瞄準範圍
	double collisionRadius; // 碰撞箱大小
	Skill skill;          // 角色技能

	// 狀態異常
	std::vector<StatusEffect> activeEffects;

	// 武器系統+天賦系統
	Weapon* primaryWeapon = nullptr;   // 主要武器
	Weapon* secondaryWeapon = nullptr; // 副武器
	std::vector<Talent> talents;       // 天賦系統

	// 建構子
	Character(int hp, int armor, int energy, double crit, int handBladeDamage,
			  double speed, int aim, double radius, Skill skill,
			  Weapon* primaryWeapon = nullptr, Weapon* secondaryWeapon = nullptr);

	// 方法
	void takeDamage(int dmg);					// 承受傷害
	void recoverArmor();						// 自動回復護甲
	void useEnergy(int amount);					// 消耗能量
	bool checkCollision(Character& other);		// 碰撞檢測
	void attack(Character& target);				// 使用當前武器攻擊敵人
	void switchWeapon();						// 切換武器
	void applyStatusEffect(const StatusEffect& effect); // 添加狀態異常
	void updateStatusEffects(double deltaTime); // 更新狀態異常
	void addTalent(const Talent& talent);		// 添加天賦
	void useSkill(Skill skill);					// 施放技能
};

#endif // CHARACTER_HPP
