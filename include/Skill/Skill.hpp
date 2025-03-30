//
// Created by QuzzS on 2025/3/4.
//

#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>
#include "Creature/Character.hpp"

class Skill {
public:
	Skill(Character& owner, std::string name, float cooldownTime): m_owner(owner), m_skillName(name), m_cooldownTime(cooldownTime) {};
	virtual ~Skill() = default;

	virtual void SkillUpdate() = 0;

	//----Getter----
	[[nodiscard]] std::string GetSkillName() const { return m_skillName; }
	[[nodiscard]] float GetCoolDownTime() const { return m_cooldownTime; }
	[[nodiscard]] float GetremainingCooldown() const { return remainingCooldown; }
	[[nodiscard]] bool GetSkillActive() const { return isActive; }

	//----Setter----
	void SetCoolDownTime(float cooldownTime) { m_cooldownTime = cooldownTime; }
	void SetSkillActive(bool active) { isActive = active; }

	virtual void Execute() = 0;

protected:
	Character& m_owner;			// 引用
	std::string m_skillName;
	float m_cooldownTime;
	float remainingCooldown;
	bool isActive = false;
};

#endif