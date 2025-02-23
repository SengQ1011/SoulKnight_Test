//
// Created by tjx20 on 2/23/2025.
//
#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>
#include "StatusEffect.hpp"

class Skill {
public:
	std::string name;			// 技能名稱
	double cooldownTime;		// 技能冷卻時間
	double remainingCooldown;	// 剩餘冷卻時間

	// 建構子
	Skill(std::string name, double cooldown, std::string effect);

	// 方法
	bool isReady() const;   // 檢查是否冷卻完畢
	void activate();        // 施放技能
	void updateCooldown(double deltaTime); // 更新冷卻時間

	virtual void SkillEffect() = 0;
};

#endif //SKILL_HPP