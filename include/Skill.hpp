//
// Created by tjx20 on 2/23/2025.
//
#ifndef SKILL_HPP
#define SKILL_HPP

#include <iostream>
#include <string>
#include <functional>
#include <utility>


enum class SkillType {
	FULL_FIRE,
	ALL_OUT_ATTACK
};

class Skill {
public:
	std::string name;              // 技能名稱
	double cooldownTime;           // 技能冷卻時間
	double remainingCooldown;      // 剩餘冷卻時間

	using SkillFunction = std::function<void()>;

	// 构造函数，初始化所有成员
	explicit Skill(std::string  skillName= "Default Skill", double cooldown= 0.0, double remainingCooldown= 0.0)
		: name(std::move(skillName)), cooldownTime(cooldown), remainingCooldown(remainingCooldown) {}

	// 建構子，初始化所有成员
	explicit Skill(std::string skillName = "Default Skill", double cooldown = 0.0, double remainingCooldown = 0.0, SkillType skillType = SkillType::FULL_FIRE)
		: name(std::move(skillName)), cooldownTime(cooldown), remainingCooldown(remainingCooldown) {

		// 根據 skillType 設定 effect 函數
		switch (skillType) {
		case SkillType::FULL_FIRE:
			effect = []{ std::cout << "Casting Full Fire!" << std::endl; };
			break;
		case SkillType::ALL_OUT_ATTACK:
			effect = []{ std::cout << "Casting All Out Attack!" << std::endl; };
			break;
		default:
			effect = []{ std::cout << "Casting Default Skill!" << std::endl; };
			break;
		}
	}

	// 執行技能效果
	void Execute() {
		if (remainingCooldown <= 0.0) {
			effect();
			remainingCooldown = cooldownTime;  // 重設冷卻時間
		} else {
			std::cout << "Skill is on cooldown!" << std::endl;
		}
	}

	// 更新冷卻時間
	void UpdateCooldown(double deltaTime) {
		if (remainingCooldown > 0.0) {
			remainingCooldown -= deltaTime;
			if (remainingCooldown < 0.0) {
				remainingCooldown = 0.0;
			}
		}
	}

private:
	SkillFunction effect;  // 技能效果的函數指針
};

#endif // SKILL_HPP