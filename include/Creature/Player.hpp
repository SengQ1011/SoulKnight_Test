//
// Created by QuzzS on 2025/3/4.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Creature/Character.hpp"
#include "GameMechanism/Skill.hpp"
#include "GameMechanism/Talent.hpp"
#include "Weapon/Weapon.hpp"

class Player : public Character {
public:
	// 構造函數：初始化玩家屬性
	Player(const std::string& ImagePath, int maxHp, float speed, int aimRange, double radius,
		   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, Skill& skill,
		   Weapon* primaryWeapon = nullptr, Weapon* secondaryWeapon = nullptr);

	// 屬性
	int maxArmor;         // 護甲上限
	int currentArmor;     // 當前護甲值
	int maxEnergy;        // 能量上限
	int currentEnergy;    // 當前能量值

	void attack(Character& target) override;	// 實現攻擊行為
	void recoverArmor();						// 自動回復護甲
	void useEnergy(int amount);					// 消耗能量
	void switchWeapon();						// 切換武器
	void changeWeapon();						// 更換新的武器
	void addTalent(const Talent& talent);		// 添加天賦
	void useSkill(Skill& skill);				// 施放技能
	void move(double deltaTime) override;		// 移動

private:
	double criticalRate;	// 暴擊率（0-1）
	int handBladeDamage;	// 手刀傷害
	Skill skill;			// 角色技能

	// 武器系統+天賦系統
	Weapon* primaryWeapon = nullptr;   // 主要武器
	Weapon* secondaryWeapon = nullptr; // 副武器
	std::vector<Talent> talents;       // 天賦系統
};

#endif //PLAYER_HPP
