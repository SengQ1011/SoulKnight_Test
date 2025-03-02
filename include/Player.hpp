//
// Created by tjx20 on 3/2/2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Character.hpp"
#include "Skill.hpp"
#include "Talent.hpp"
#include "Weapon.hpp"
#include "Skill.hpp"

class Player : public Character {
public:
	// 構造函數：初始化玩家屬性
	Player(const std::string& ImagePath, int maxHp, double speed, int aimRange, double radius,
		   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage,
		   Weapon* primaryWeapon = nullptr, Weapon* secondaryWeapon = nullptr, const Skill& skill);

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
	void useSkill(Skill& skill);			// 施放技能

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
